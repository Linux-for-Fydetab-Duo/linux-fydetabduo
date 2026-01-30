.. SPDX-License-Identifier: GPL-2.0

==============================
Revocable Resource Management
==============================

Overview
========

In a system with hot-pluggable devices, such as USB, resources provided by
these devices can be removed asynchronously.  If a consumer holds a reference
to such a resource, the resource might be deallocated while the reference is
still held, leading to use-after-free errors upon subsequent access.

The "revocable" mechanism addresses this by establishing a weak reference to a
resource that might be freed at any time.  It allows a resource consumer to
safely attempt to access the resource, guaranteeing that the access is valid
for the duration of its use, or it fails safely if the resource has already
been revoked.

The implementation is based on a provider/consumer model that uses Sleepable
RCU (SRCU) to ensure safe memory access without traditional locking.

How It Works
============

1.  **Provider**: A resource provider, such as a driver for a hot-pluggable
    device, allocates a ``struct revocable_provider``.  This structure is
    initialized with a pointer to the actual resource it manages.

2.  **Consumer**: A consumer that needs to access the resource is given a
    ``struct revocable``, which acts as a handle containing a reference to
    the provider.

3.  **Accessing the Resource**: To access the resource, the consumer uses
    ``revocable_try_access()``.  This function enters an SRCU read-side
    critical section and returns a pointer to the resource.  If the provider
    has already revoked the resource, this function returns ``NULL``.  The
    consumer must check for this ``NULL`` return.

4.  **Releasing the Resource**: After the consumer has finished using the
    resource, it must call ``revocable_release()`` to exit the SRCU critical
    section.  This signals that the consumer no longer requires access.  The
    ``REVOCABLE()`` macro is provided as a convenient and safe way to manage
    the access-release cycle.

5.  **Revoking the Resource**: When the provider needs to remove the resource
    (e.g., the device is unplugged), it calls ``revocable_provider_free()``.
    This function first sets the internal resource pointer to ``NULL``,
    preventing any new consumers from accessing it.  It then calls
    ``synchronize_srcu()``, which waits for all existing consumers currently
    in the SRCU critical section to finish their work.  Once all consumers
    have released their access, the resource can be safely deallocated.

Revocable vs. Device-Managed (devm) Resources
=============================================

It's important to understand the distinction between a standard
device-managed (devm) resource and a resource managed by a
``revocable_provider``.

The key difference is their lifetime:

*   A **devm resource** is tied to the lifetime of the device.  It is
    automatically freed when the device is unbound.
*   A **revocable_provider** persists as long as there are active references
    to it from ``revocable`` consumer handles.

This means that a ``revocable_provider`` can outlive the device that created
it.  This is a deliberate design feature, allowing consumers to hold a
reference to a resource even after the underlying device has been removed,
without causing a fault.  When the consumer attempts to access the resource,
it will simply be informed that the resource is no longer available.

API and Usage
=============

For Resource Providers
----------------------

``struct revocable_provider *revocable_provider_alloc(void *res);``
    Allocates a provider handle for the given resource ``res``.  It returns a
    pointer to the ``revocable_provider`` on success, or ``NULL`` on failure.

``struct revocable_provider *devm_revocable_provider_alloc(struct device *dev, void *res);``
    A device-managed version of ``revocable_provider_alloc``.  It is
    convenient to allocate providers via this function if the ``res`` is also
    tied to the lifetime of the ``dev``.  ``revocable_provider_free`` will be
    called automatically when the device is unbound.

``void revocable_provider_free(struct revocable_provider *rp);``
    Revokes the resource.  This function marks the resource as unavailable and
    waits for all current consumers to finish before the underlying memory
    can be freed.

For Resource Consumers
----------------------

``struct revocable *revocable_alloc(struct revocable_provider *rp);``
    Allocates a consumer handle for a given provider ``rp``.

``void revocable_free(struct revocable *rev);``
    Frees a consumer handle.

``void *revocable_try_access(struct revocable *rev);``
    Attempts to gain access to the resource.  Returns a pointer to the
    resource on success or ``NULL`` if it has been revoked.

``void revocable_release(struct revocable *rev);``
    Releases access to the resource, exiting the SRCU critical section.

The ``REVOCABLE()`` Macro
=========================

The ``REVOCABLE()`` macro simplifies the access-release cycle for consumers,
ensuring that ``revocable_release()`` is always called, even in the case of
an early exit.

``REVOCABLE(rev, res)``
    *   ``rev``: The consumer's ``struct revocable *`` handle.
    *   ``res``: A pointer variable that will be assigned the resource.

The macro creates a ``for`` loop that executes exactly once.  Inside the loop,
``res`` is populated with the result of ``revocable_try_access()``.  The
consumer code **must** check if ``res`` is ``NULL`` before using it.  The
``revocable_release()`` function is automatically called when the scope of
the loop is exited.

Example Usage
-------------

.. code-block:: c

    void consumer_use_resource(struct revocable *rev)
    {
        struct foo_resource *res;

        REVOCABLE(rev, res) {
            // Always check if the resource is valid.
            if (!res) {
                pr_warn("Resource is not available\n");
                return;
            }

            // At this point, 'res' is guaranteed to be valid until
            // this block exits.
            do_something_with(res);
        }

        // revocable_release() is automatically called here.
    }
