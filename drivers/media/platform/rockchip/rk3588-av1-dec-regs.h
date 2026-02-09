/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Rockchip RK3588 AV1 Decoder - VDPU383 Register Definitions
 *
 * Copyright (C) 2026
 *
 * Ported from Rockchip MPP library (Apache-2.0 OR MIT license)
 * Original file: mpp/hal/rkdec/inc/vdpu383_av1d.h
 */

#ifndef __RK3588_AV1_DEC_REGS_H__
#define __RK3588_AV1_DEC_REGS_H__

#include <linux/types.h>

/*
 * VDPU383 AV1 Decoder Register Structures
 * Ported from MPP library for RK3588 hardware
 */

/* AV1 Decoder Parameters (registers 64-106) */
struct vdpu383_av1d_params {
	/* SWREG64 */
	u32 reg64_unused;

	/* SWREG65 - Stream parameter set */
	u32 reg65_strm_start_bit;

	/* SWREG66 - Stream length */
	u32 reg66_stream_len;

	/* SWREG67 - Global (uncompressed header) length */
	u32 reg67_global_len;

	/* SWREG68 - Horizontal stride */
	u32 reg68_hor_virstride;

	/* SWREG69 - Raster UV horizontal stride */
	u32 reg69_raster_uv_hor_virstride;

	/* SWREG70 - Y stride */
	u32 reg70_y_virstride;

	/* SWREG71-73 - Scaled reference strides */
	u32 reg71_scl_ref_hor_virstride;
	u32 reg72_scl_ref_raster_uv_hor_virstride;
	u32 reg73_scl_ref_virstride;

	/* SWREG74 - Film grain synthesis stride */
	u32 reg74_fgs_ref_hor_virstride;

	u32 reserve_reg75_79[5];

	/* SWREG80-82 - Error reference strides */
	u32 reg80_error_ref_hor_virstride;
	u32 reg81_error_ref_raster_uv_hor_virstride;
	u32 reg82_error_ref_virstride;

	/* SWREG83-106 - Reference frame strides (8 reference frames × 3 values) */
	u32 reg83_ref0_hor_virstride;
	u32 reg84_ref0_raster_uv_hor_virstride;
	u32 reg85_ref0_virstride;

	u32 reg86_ref1_hor_virstride;
	u32 reg87_ref1_raster_uv_hor_virstride;
	u32 reg88_ref1_virstride;

	u32 reg89_ref2_hor_virstride;
	u32 reg90_ref2_raster_uv_hor_virstride;
	u32 reg91_ref2_virstride;

	u32 reg92_ref3_hor_virstride;
	u32 reg93_ref3_raster_uv_hor_virstride;
	u32 reg94_ref3_virstride;

	u32 reg95_ref4_hor_virstride;
	u32 reg96_ref4_raster_uv_hor_virstride;
	u32 reg97_ref4_virstride;

	u32 reg98_ref5_hor_virstride;
	u32 reg99_ref5_raster_uv_hor_virstride;
	u32 reg100_ref5_virstride;

	u32 reg101_ref6_hor_virstride;
	u32 reg102_ref6_raster_uv_hor_virstride;
	u32 reg103_ref6_virstride;

	u32 reg104_ref7_hor_virstride;
	u32 reg105_ref7_raster_uv_hor_virstride;
	u32 reg106_ref7_virstride;
} __packed;

/* AV1 Decoder Addresses (registers 168-232) */
struct vdpu383_av1d_addrs {
	/* SWREG168 - Decoded output base */
	u32 reg168_decout_base;

	/* SWREG169 - Error reference base */
	u32 reg169_error_ref_base;

	/* SWREG170-177 - Reference frame bases (8 frames) */
	u32 reg170_av1_last_base;     /* REF0 */
	u32 reg171_av1golden_base;    /* REF1 */
	u32 reg172_av1alfter_base;    /* REF2 */
	u32 reg173_refer3_base;
	u32 reg174_refer4_base;
	u32 reg175_refer5_base;
	u32 reg176_refer6_base;
	u32 reg177_refer7_base;

	u32 reserve_reg178_191[14];

	/* SWREG192 - FBC payload (current frame) base */
	u32 reg192_payload_st_cur_base;

	/* SWREG193 - FBC payload offset */
	u32 reg193_fbc_payload_offset;

	/* SWREG194-201 - FBC payload reference bases (8 frames) */
	u32 reg194_payload_st_ref0_base;
	u32 reg195_payload_st_ref1_base;
	u32 reg196_payload_st_ref2_base;
	u32 reg197_payload_st_ref3_base;
	u32 reg198_payload_st_ref4_base;
	u32 reg199_payload_st_ref5_base;
	u32 reg200_payload_st_ref6_base;
	u32 reg201_payload_st_ref7_base;

	u32 reserve_reg202_215[14];

	/* SWREG216 - Collocated MV (current frame) base */
	u32 reg216_colmv_cur_base;

	/* SWREG217-232 - Collocated MV reference bases (16 frames) */
	u32 reg217_232_colmv_ref_base[16];
} __packed;

/* Control registers (registers 8-30) */
struct vdpu383_ctrl_regs {
	/* SWREG8 - Decoder mode */
	u32 reg8_dec_mode;

	/* SWREG9 - Decoder configuration */
	struct {
		u32 fbc_e : 1;
		u32 tile_e : 1;
		u32 scale_down_en : 1;
		u32 buf_empty_en : 1;
		u32 reserved : 28;
	} reg9;

	/* SWREG10 - Auto-gating controls */
	struct {
		u32 strmd_auto_gating_e : 1;
		u32 inter_auto_gating_e : 1;
		u32 intra_auto_gating_e : 1;
		u32 transd_auto_gating_e : 1;
		u32 recon_auto_gating_e : 1;
		u32 filterd_auto_gating_e : 1;
		u32 bus_auto_gating_e : 1;
		u32 ctrl_auto_gating_e : 1;
		u32 rcb_auto_gating_e : 1;
		u32 err_prc_auto_gating_e : 1;
		u32 reserved : 22;
	} reg10;

	/* SWREG11 - Timeout control */
	struct {
		u32 dec_timeout_dis : 1;
		u32 reserved : 31;
	} reg11;

	u32 reg12_reserved;

	/* SWREG13 - Core timeout threshold */
	u32 reg13_core_timeout_threshold;

	u32 reg14_15_reserved[2];

	/* SWREG16 - Error processing */
	struct {
		u32 error_proc_disable : 1;
		u32 error_spread_disable : 1;
		u32 roi_error_ctu_cal_en : 1;
		u32 reserved : 29;
	} reg16;

	u32 reg17_19_reserved[3];

	/* SWREG20-21 - CABAC error enable */
	u32 reg20_cabac_error_en_lowbits;
	u32 reg21_cabac_error_en_highbits;

	u32 reg22_27_reserved[6];

	/* SWREG28 - AXI performance */
	struct {
		u32 axi_perf_work_e : 1;
		u32 axi_cnt_type : 1;
		u32 rd_latency_id : 5;
		u32 reserved : 25;
	} reg28;

	/* SWREG29 - AXI counter configuration */
	struct {
		u32 addr_align_type : 2;
		u32 ar_cnt_id_type : 1;
		u32 aw_cnt_id_type : 1;
		u32 ar_count_id : 8;
		u32 aw_count_id : 8;
		u32 rd_band_width_mode : 1;
		u32 reserved : 11;
	} reg29;

	/* SWREG30 - AXI QoS */
	struct {
		u32 axi_wr_qos : 4;
		u32 axi_rd_qos : 4;
		u32 reserved : 24;
	} reg30;
} __packed;

/* Common address registers (registers 128-161) */
struct vdpu383_common_addrs {
	/* SWREG128 - Stream base */
	u32 reg128_strm_base;

	u32 reg129_130_reserved[2];

	/* SWREG131 - Global (uncompressed header) base */
	u32 reg131_gbl_base;

	/* SWREG132 - RCB (Reconstruction Buffer) base */
	u32 reg132_scanlist_addr;

	/* SWREG133 - Scale down base */
	u32 reg133_scale_down_base;

	u32 reg134_161_reserved[28];
} __packed;

/* Complete register set for VDPU383 AV1 Decoder */
struct vdpu383_av1d_reg_set {
	u32 reg0_7_reserved[8];
	struct vdpu383_ctrl_regs ctrl_regs;         /* regs 8-30 */
	u32 reg31_63_reserved[33];
	struct vdpu383_av1d_params av1d_params;     /* regs 64-106 */
	u32 reg107_127_reserved[21];
	struct vdpu383_common_addrs common_addrs;   /* regs 128-161 */
	u32 reg162_167_reserved[6];
	struct vdpu383_av1d_addrs av1d_addrs;       /* regs 168-232 */
} __packed;

#endif /* __RK3588_AV1_DEC_REGS_H__ */
