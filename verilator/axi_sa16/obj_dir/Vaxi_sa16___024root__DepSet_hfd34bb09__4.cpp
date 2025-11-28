// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vaxi_sa16.h for the primary calling header

#include "verilated.h"

#include "Vaxi_sa16___024root.h"

VL_INLINE_OPT void Vaxi_sa16___024root___sequent__TOP__7(Vaxi_sa16___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vaxi_sa16__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaxi_sa16___024root___sequent__TOP__7\n"); );
    // Init
    IData/*31:0*/ axi_sa16__DOT__i;
    IData/*31:0*/ __Vdly__axi_sa16__DOT__counter;
    CData/*0:0*/ __Vdly__axi_sa16__DOT__bank_load;
    CData/*6:0*/ __Vdly__axi_sa16__DOT__load_count;
    CData/*0:0*/ __Vdly__axi_sa16__DOT__tile_ready0;
    CData/*0:0*/ __Vdly__axi_sa16__DOT__tile_ready1;
    CData/*3:0*/ __Vdlyvdim0__axi_sa16__DOT__inputA_bank1__v0;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__inputA_bank1__v0;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__inputA_bank1__v0;
    CData/*3:0*/ __Vdlyvdim0__axi_sa16__DOT__inputA_bank0__v0;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__inputA_bank0__v0;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__inputA_bank0__v0;
    CData/*5:0*/ __Vdlyvdim0__axi_sa16__DOT__inputB_bank1__v0;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__inputB_bank1__v0;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__inputB_bank1__v0;
    CData/*5:0*/ __Vdlyvdim0__axi_sa16__DOT__inputB_bank0__v0;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__inputB_bank0__v0;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__inputB_bank0__v0;
    CData/*1:0*/ __Vdly__axi_sa16__DOT__sa_state;
    CData/*0:0*/ __Vdly__axi_sa16__DOT__bank_comp;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v0;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__A_buf__v0;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v1;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__A_buf__v1;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v2;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v3;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v4;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v5;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v6;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v7;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v8;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v9;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v10;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v11;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v12;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v13;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v14;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v15;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v16;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__A_buf__v16;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v17;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__A_buf__v17;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v18;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v19;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v20;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v21;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v22;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v23;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v24;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v25;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v26;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v27;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v28;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v29;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v30;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__A_buf__v31;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v0;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__B_buf__v0;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v1;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__B_buf__v1;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v2;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v3;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v4;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v5;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v6;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v7;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v8;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v9;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v10;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v11;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v12;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v13;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v14;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v15;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v16;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v17;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v18;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v19;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v20;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v21;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v22;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v23;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v24;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v25;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v26;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v27;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v28;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v29;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v30;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v31;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v32;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v33;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v34;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v35;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v36;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v37;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v38;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v39;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v40;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v41;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v42;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v43;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v44;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v45;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v46;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v47;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v48;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v49;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v50;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v51;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v52;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v53;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v54;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v55;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v56;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v57;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v58;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v59;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v60;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v61;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v62;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__B_buf__v63;
    CData/*6:0*/ __Vdly__axi_sa16__DOT__warmup_cycles;
    SData/*9:0*/ __Vdly__axi_sa16__DOT__compute_cycles;
    CData/*5:0*/ __Vdly__axi_sa16__DOT__stable_cycles;
    CData/*0:0*/ __Vdly__axi_sa16__DOT__final_output_detected;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__prev_outputC__v0;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__prev_outputC__v64;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__prev_outputC__v65;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__prev_outputC__v66;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v128;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__prev_outputC__v128;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v129;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v130;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v131;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v132;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v133;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v134;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v135;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v136;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v137;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v138;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v139;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v140;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v141;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v142;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v143;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v144;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v145;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v146;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v147;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v148;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v149;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v150;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v151;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v152;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v153;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v154;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v155;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v156;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v157;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v158;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v159;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v160;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v161;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v162;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v163;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v164;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v165;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v166;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v167;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v168;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v169;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v170;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v171;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v172;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v173;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v174;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v175;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v176;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v177;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v178;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v179;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v180;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v181;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v182;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v183;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v184;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v185;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v186;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v187;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v188;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v189;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v190;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__prev_outputC__v191;
    CData/*0:0*/ __Vdly__axi_sa16__DOT__b_loaded;
    CData/*3:0*/ __Vdlyvdim0__axi_sa16__DOT__inputA_bank1__v1;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__inputA_bank1__v1;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__inputA_bank1__v1;
    CData/*3:0*/ __Vdlyvdim0__axi_sa16__DOT__inputA_bank0__v1;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__inputA_bank0__v1;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__inputA_bank0__v1;
    CData/*5:0*/ __Vdlyvdim0__axi_sa16__DOT__inputB_bank0__v1;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__inputB_bank0__v1;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__inputB_bank0__v1;
    CData/*5:0*/ __Vdlyvdim0__axi_sa16__DOT__inputB_bank1__v1;
    CData/*7:0*/ __Vdlyvval__axi_sa16__DOT__inputB_bank1__v1;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__bypassC__v0;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__bypassC__v64;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v128;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__bypassC__v128;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v129;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v130;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v131;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v132;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v133;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v134;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v135;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v136;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v137;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v138;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v139;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v140;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v141;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v142;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v143;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v144;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v145;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v146;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v147;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v148;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v149;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v150;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v151;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v152;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v153;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v154;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v155;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v156;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v157;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v158;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v159;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v160;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v161;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v162;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v163;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v164;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v165;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v166;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v167;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v168;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v169;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v170;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v171;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v172;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v173;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v174;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v175;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v176;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v177;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v178;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v179;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v180;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v181;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v182;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v183;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v184;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v185;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v186;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v187;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v188;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v189;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v190;
    IData/*19:0*/ __Vdlyvval__axi_sa16__DOT__bypassC__v191;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__output_mem__v0;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v64;
    CData/*0:0*/ __Vdlyvset__axi_sa16__DOT__output_mem__v64;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v65;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v66;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v67;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v68;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v69;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v70;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v71;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v72;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v73;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v74;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v75;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v76;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v77;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v78;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v79;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v80;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v81;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v82;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v83;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v84;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v85;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v86;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v87;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v88;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v89;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v90;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v91;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v92;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v93;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v94;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v95;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v96;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v97;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v98;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v99;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v100;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v101;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v102;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v103;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v104;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v105;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v106;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v107;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v108;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v109;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v110;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v111;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v112;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v113;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v114;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v115;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v116;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v117;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v118;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v119;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v120;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v121;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v122;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v123;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v124;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v125;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v126;
    IData/*31:0*/ __Vdlyvval__axi_sa16__DOT__output_mem__v127;
    CData/*0:0*/ __Vdly__axi_sa16__DOT__print_pending;
    IData/*31:0*/ __Vilp;
    // Body
    __Vdly__axi_sa16__DOT__counter = vlSelf->axi_sa16__DOT__counter;
    __Vdly__axi_sa16__DOT__print_pending = vlSelf->axi_sa16__DOT__print_pending;
    __Vdly__axi_sa16__DOT__b_loaded = vlSelf->axi_sa16__DOT__b_loaded;
    __Vdlyvset__axi_sa16__DOT__inputB_bank1__v0 = 0U;
    __Vdly__axi_sa16__DOT__load_count = vlSelf->axi_sa16__DOT__load_count;
    __Vdly__axi_sa16__DOT__bank_load = vlSelf->axi_sa16__DOT__bank_load;
    __Vdlyvset__axi_sa16__DOT__inputA_bank0__v0 = 0U;
    __Vdlyvset__axi_sa16__DOT__inputA_bank0__v1 = 0U;
    __Vdlyvset__axi_sa16__DOT__inputA_bank1__v0 = 0U;
    __Vdlyvset__axi_sa16__DOT__inputA_bank1__v1 = 0U;
    __Vdlyvset__axi_sa16__DOT__inputB_bank0__v0 = 0U;
    __Vdlyvset__axi_sa16__DOT__inputB_bank0__v1 = 0U;
    __Vdlyvset__axi_sa16__DOT__output_mem__v0 = 0U;
    __Vdlyvset__axi_sa16__DOT__output_mem__v64 = 0U;
    __Vdly__axi_sa16__DOT__bank_comp = vlSelf->axi_sa16__DOT__bank_comp;
    __Vdly__axi_sa16__DOT__sa_state = vlSelf->axi_sa16__DOT__sa_state;
    __Vdly__axi_sa16__DOT__compute_cycles = vlSelf->axi_sa16__DOT__compute_cycles;
    __Vdly__axi_sa16__DOT__tile_ready1 = vlSelf->axi_sa16__DOT__tile_ready1;
    __Vdly__axi_sa16__DOT__tile_ready0 = vlSelf->axi_sa16__DOT__tile_ready0;
    __Vdly__axi_sa16__DOT__warmup_cycles = vlSelf->axi_sa16__DOT__warmup_cycles;
    __Vdly__axi_sa16__DOT__stable_cycles = vlSelf->axi_sa16__DOT__stable_cycles;
    __Vdly__axi_sa16__DOT__final_output_detected = vlSelf->axi_sa16__DOT__final_output_detected;
    __Vdlyvset__axi_sa16__DOT__B_buf__v0 = 0U;
    __Vdlyvset__axi_sa16__DOT__B_buf__v1 = 0U;
    __Vdlyvset__axi_sa16__DOT__bypassC__v0 = 0U;
    __Vdlyvset__axi_sa16__DOT__bypassC__v64 = 0U;
    __Vdlyvset__axi_sa16__DOT__bypassC__v128 = 0U;
    __Vdlyvset__axi_sa16__DOT__A_buf__v0 = 0U;
    __Vdlyvset__axi_sa16__DOT__A_buf__v1 = 0U;
    __Vdlyvset__axi_sa16__DOT__A_buf__v16 = 0U;
    __Vdlyvset__axi_sa16__DOT__A_buf__v17 = 0U;
    __Vdlyvset__axi_sa16__DOT__prev_outputC__v0 = 0U;
    __Vdlyvset__axi_sa16__DOT__prev_outputC__v64 = 0U;
    __Vdlyvset__axi_sa16__DOT__prev_outputC__v65 = 0U;
    __Vdlyvset__axi_sa16__DOT__prev_outputC__v66 = 0U;
    __Vdlyvset__axi_sa16__DOT__prev_outputC__v128 = 0U;
    vlSelf->axi_sa16__DOT__final_q = ((~ (IData)(vlSelf->rst)) 
                                      & (IData)(vlSelf->axi_sa16__DOT__final_output_detected));
    __Vdly__axi_sa16__DOT__counter = ((IData)(vlSelf->rst)
                                       ? 0U : ((IData)(1U) 
                                               + vlSelf->axi_sa16__DOT__counter));
    if (vlSelf->rst) {
        __Vdly__axi_sa16__DOT__print_pending = 0U;
    } else if (vlSelf->axi_sa16__DOT__final_rise) {
        __Vdly__axi_sa16__DOT__print_pending = 1U;
    } else if (VL_UNLIKELY(vlSelf->axi_sa16__DOT__print_pending)) {
        VL_WRITEF("\nComputation Complete. OutputC Values:\n%08x %08x %08x %08x %08x %08x %08x %08x\n",
                  32,vlSelf->axi_sa16__DOT__output_mem
                  [0U],32,vlSelf->axi_sa16__DOT__output_mem
                  [1U],32,vlSelf->axi_sa16__DOT__output_mem
                  [2U],32,vlSelf->axi_sa16__DOT__output_mem
                  [3U],32,vlSelf->axi_sa16__DOT__output_mem
                  [4U],32,vlSelf->axi_sa16__DOT__output_mem
                  [5U],32,vlSelf->axi_sa16__DOT__output_mem
                  [6U],32,vlSelf->axi_sa16__DOT__output_mem
                  [7U]);
        __Vdly__axi_sa16__DOT__print_pending = 0U;
        VL_WRITEF("%08x %08x %08x %08x %08x %08x %08x %08x\n%08x %08x %08x %08x %08x %08x %08x %08x\n%08x %08x %08x %08x %08x %08x %08x %08x\n%08x %08x %08x %08x %08x %08x %08x %08x\n%08x %08x %08x %08x %08x %08x %08x %08x\n%08x %08x %08x %08x %08x %08x %08x %08x\n%08x %08x %08x %08x %08x %08x %08x %08x\n",
                  32,vlSelf->axi_sa16__DOT__output_mem
                  [8U],32,vlSelf->axi_sa16__DOT__output_mem
                  [9U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0xaU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0xbU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0xcU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0xdU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0xeU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0xfU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x10U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x11U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x12U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x13U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x14U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x15U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x16U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x17U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x18U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x19U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x1aU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x1bU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x1cU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x1dU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x1eU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x1fU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x20U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x21U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x22U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x23U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x24U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x25U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x26U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x27U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x28U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x29U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x2aU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x2bU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x2cU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x2dU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x2eU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x2fU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x30U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x31U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x32U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x33U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x34U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x35U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x36U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x37U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x38U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x39U],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x3aU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x3bU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x3cU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x3dU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x3eU],32,vlSelf->axi_sa16__DOT__output_mem
                  [0x3fU]);
    }
    if (vlSelf->rst) {
        axi_sa16__DOT__i = 0x40U;
        __Vdlyvset__axi_sa16__DOT__output_mem__v0 = 1U;
    } else if (VL_UNLIKELY(vlSelf->axi_sa16__DOT__final_rise)) {
        axi_sa16__DOT__i = 0x40U;
        VL_WRITEF("[%t] Tile written to output_mem[0..63]\n",
                  64,VL_TIME_UNITED_Q(1),-12);
        __Vdlyvval__axi_sa16__DOT__output_mem__v64 
            = vlSelf->axi_sa16__DOT__bypassC[0U];
        __Vdlyvset__axi_sa16__DOT__output_mem__v64 = 1U;
        __Vdlyvval__axi_sa16__DOT__output_mem__v65 
            = vlSelf->axi_sa16__DOT__bypassC[1U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v66 
            = vlSelf->axi_sa16__DOT__bypassC[2U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v67 
            = vlSelf->axi_sa16__DOT__bypassC[3U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v68 
            = vlSelf->axi_sa16__DOT__bypassC[4U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v69 
            = vlSelf->axi_sa16__DOT__bypassC[5U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v70 
            = vlSelf->axi_sa16__DOT__bypassC[6U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v71 
            = vlSelf->axi_sa16__DOT__bypassC[7U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v72 
            = vlSelf->axi_sa16__DOT__bypassC[8U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v73 
            = vlSelf->axi_sa16__DOT__bypassC[9U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v74 
            = vlSelf->axi_sa16__DOT__bypassC[0xaU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v75 
            = vlSelf->axi_sa16__DOT__bypassC[0xbU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v76 
            = vlSelf->axi_sa16__DOT__bypassC[0xcU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v77 
            = vlSelf->axi_sa16__DOT__bypassC[0xdU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v78 
            = vlSelf->axi_sa16__DOT__bypassC[0xeU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v79 
            = vlSelf->axi_sa16__DOT__bypassC[0xfU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v80 
            = vlSelf->axi_sa16__DOT__bypassC[0x10U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v81 
            = vlSelf->axi_sa16__DOT__bypassC[0x11U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v82 
            = vlSelf->axi_sa16__DOT__bypassC[0x12U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v83 
            = vlSelf->axi_sa16__DOT__bypassC[0x13U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v84 
            = vlSelf->axi_sa16__DOT__bypassC[0x14U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v85 
            = vlSelf->axi_sa16__DOT__bypassC[0x15U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v86 
            = vlSelf->axi_sa16__DOT__bypassC[0x16U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v87 
            = vlSelf->axi_sa16__DOT__bypassC[0x17U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v88 
            = vlSelf->axi_sa16__DOT__bypassC[0x18U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v89 
            = vlSelf->axi_sa16__DOT__bypassC[0x19U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v90 
            = vlSelf->axi_sa16__DOT__bypassC[0x1aU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v91 
            = vlSelf->axi_sa16__DOT__bypassC[0x1bU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v92 
            = vlSelf->axi_sa16__DOT__bypassC[0x1cU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v93 
            = vlSelf->axi_sa16__DOT__bypassC[0x1dU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v94 
            = vlSelf->axi_sa16__DOT__bypassC[0x1eU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v95 
            = vlSelf->axi_sa16__DOT__bypassC[0x1fU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v96 
            = vlSelf->axi_sa16__DOT__bypassC[0x20U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v97 
            = vlSelf->axi_sa16__DOT__bypassC[0x21U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v98 
            = vlSelf->axi_sa16__DOT__bypassC[0x22U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v99 
            = vlSelf->axi_sa16__DOT__bypassC[0x23U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v100 
            = vlSelf->axi_sa16__DOT__bypassC[0x24U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v101 
            = vlSelf->axi_sa16__DOT__bypassC[0x25U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v102 
            = vlSelf->axi_sa16__DOT__bypassC[0x26U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v103 
            = vlSelf->axi_sa16__DOT__bypassC[0x27U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v104 
            = vlSelf->axi_sa16__DOT__bypassC[0x28U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v105 
            = vlSelf->axi_sa16__DOT__bypassC[0x29U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v106 
            = vlSelf->axi_sa16__DOT__bypassC[0x2aU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v107 
            = vlSelf->axi_sa16__DOT__bypassC[0x2bU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v108 
            = vlSelf->axi_sa16__DOT__bypassC[0x2cU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v109 
            = vlSelf->axi_sa16__DOT__bypassC[0x2dU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v110 
            = vlSelf->axi_sa16__DOT__bypassC[0x2eU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v111 
            = vlSelf->axi_sa16__DOT__bypassC[0x2fU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v112 
            = vlSelf->axi_sa16__DOT__bypassC[0x30U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v113 
            = vlSelf->axi_sa16__DOT__bypassC[0x31U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v114 
            = vlSelf->axi_sa16__DOT__bypassC[0x32U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v115 
            = vlSelf->axi_sa16__DOT__bypassC[0x33U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v116 
            = vlSelf->axi_sa16__DOT__bypassC[0x34U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v117 
            = vlSelf->axi_sa16__DOT__bypassC[0x35U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v118 
            = vlSelf->axi_sa16__DOT__bypassC[0x36U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v119 
            = vlSelf->axi_sa16__DOT__bypassC[0x37U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v120 
            = vlSelf->axi_sa16__DOT__bypassC[0x38U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v121 
            = vlSelf->axi_sa16__DOT__bypassC[0x39U];
        __Vdlyvval__axi_sa16__DOT__output_mem__v122 
            = vlSelf->axi_sa16__DOT__bypassC[0x3aU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v123 
            = vlSelf->axi_sa16__DOT__bypassC[0x3bU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v124 
            = vlSelf->axi_sa16__DOT__bypassC[0x3cU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v125 
            = vlSelf->axi_sa16__DOT__bypassC[0x3dU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v126 
            = vlSelf->axi_sa16__DOT__bypassC[0x3eU];
        __Vdlyvval__axi_sa16__DOT__output_mem__v127 
            = vlSelf->axi_sa16__DOT__bypassC[0x3fU];
    }
    if (vlSelf->rst) {
        __Vdly__axi_sa16__DOT__compute_cycles = 0U;
        __Vdly__axi_sa16__DOT__bank_load = 0U;
        __Vdly__axi_sa16__DOT__load_count = 0U;
        __Vdly__axi_sa16__DOT__tile_ready0 = 0U;
        __Vdly__axi_sa16__DOT__tile_ready1 = 0U;
        __Vdly__axi_sa16__DOT__b_loaded = 0U;
        __Vdly__axi_sa16__DOT__bank_load = 0U;
        __Vdly__axi_sa16__DOT__load_count = 0U;
        __Vdly__axi_sa16__DOT__tile_ready0 = 0U;
        __Vdly__axi_sa16__DOT__tile_ready1 = 0U;
        __Vdly__axi_sa16__DOT__warmup_cycles = 0U;
        __Vdlyvset__axi_sa16__DOT__bypassC__v0 = 1U;
    } else {
        if (vlSelf->axi_sa16__DOT__sa_start_pulse) {
            __Vdly__axi_sa16__DOT__compute_cycles = 0U;
            __Vdly__axi_sa16__DOT__warmup_cycles = 0x40U;
            __Vdlyvset__axi_sa16__DOT__bypassC__v64 = 1U;
        } else {
            if (((IData)(vlSelf->axi_sa16__DOT__sa_busy) 
                 & (0U != (IData)(vlSelf->axi_sa16__DOT__warmup_cycles)))) {
                __Vdly__axi_sa16__DOT__warmup_cycles 
                    = (0x7fU & ((IData)(vlSelf->axi_sa16__DOT__warmup_cycles) 
                                - (IData)(1U)));
            }
            if ((((2U == (IData)(vlSelf->axi_sa16__DOT__sa_state)) 
                  & (~ (IData)(vlSelf->axi_sa16__DOT__final_output_detected))) 
                 & (0U == (IData)(vlSelf->axi_sa16__DOT__warmup_cycles)))) {
                __Vdlyvval__axi_sa16__DOT__bypassC__v128 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0U] + (vlSelf->axi_sa16__DOT__A_buf
                                           [0U] * vlSelf->axi_sa16__DOT__B_buf
                                           [0U])));
                __Vdlyvset__axi_sa16__DOT__bypassC__v128 = 1U;
                __Vdlyvval__axi_sa16__DOT__bypassC__v129 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [1U] + (vlSelf->axi_sa16__DOT__A_buf
                                           [1U] * vlSelf->axi_sa16__DOT__B_buf
                                           [1U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v130 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [2U] + (vlSelf->axi_sa16__DOT__A_buf
                                           [2U] * vlSelf->axi_sa16__DOT__B_buf
                                           [2U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v131 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [3U] + (vlSelf->axi_sa16__DOT__A_buf
                                           [3U] * vlSelf->axi_sa16__DOT__B_buf
                                           [3U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v132 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [4U] + (vlSelf->axi_sa16__DOT__A_buf
                                           [4U] * vlSelf->axi_sa16__DOT__B_buf
                                           [4U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v133 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [5U] + (vlSelf->axi_sa16__DOT__A_buf
                                           [5U] * vlSelf->axi_sa16__DOT__B_buf
                                           [5U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v134 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [6U] + (vlSelf->axi_sa16__DOT__A_buf
                                           [6U] * vlSelf->axi_sa16__DOT__B_buf
                                           [6U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v135 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [7U] + (vlSelf->axi_sa16__DOT__A_buf
                                           [7U] * vlSelf->axi_sa16__DOT__B_buf
                                           [7U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v136 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [8U] + (vlSelf->axi_sa16__DOT__A_buf
                                           [8U] * vlSelf->axi_sa16__DOT__B_buf
                                           [8U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v137 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [9U] + (vlSelf->axi_sa16__DOT__A_buf
                                           [9U] * vlSelf->axi_sa16__DOT__B_buf
                                           [9U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v138 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0xaU] + (vlSelf->axi_sa16__DOT__A_buf
                                             [0xaU] 
                                             * vlSelf->axi_sa16__DOT__B_buf
                                             [0xaU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v139 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0xbU] + (vlSelf->axi_sa16__DOT__A_buf
                                             [0xbU] 
                                             * vlSelf->axi_sa16__DOT__B_buf
                                             [0xbU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v140 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0xcU] + (vlSelf->axi_sa16__DOT__A_buf
                                             [0xcU] 
                                             * vlSelf->axi_sa16__DOT__B_buf
                                             [0xcU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v141 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0xdU] + (vlSelf->axi_sa16__DOT__A_buf
                                             [0xdU] 
                                             * vlSelf->axi_sa16__DOT__B_buf
                                             [0xdU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v142 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0xeU] + (vlSelf->axi_sa16__DOT__A_buf
                                             [0xeU] 
                                             * vlSelf->axi_sa16__DOT__B_buf
                                             [0xeU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v143 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0xfU] + (vlSelf->axi_sa16__DOT__A_buf
                                             [0xfU] 
                                             * vlSelf->axi_sa16__DOT__B_buf
                                             [0xfU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v144 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x10U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x10U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v145 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x11U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [1U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x11U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v146 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x12U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [2U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x12U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v147 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x13U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [3U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x13U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v148 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x14U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [4U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x14U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v149 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x15U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [5U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x15U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v150 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x16U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [6U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x16U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v151 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x17U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [7U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x17U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v152 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x18U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [8U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x18U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v153 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x19U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [9U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x19U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v154 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x1aU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xaU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x1aU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v155 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x1bU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xbU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x1bU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v156 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x1cU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xcU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x1cU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v157 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x1dU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xdU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x1dU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v158 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x1eU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xeU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x1eU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v159 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x1fU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xfU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x1fU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v160 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x20U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x20U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v161 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x21U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [1U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x21U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v162 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x22U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [2U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x22U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v163 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x23U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [3U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x23U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v164 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x24U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [4U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x24U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v165 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x25U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [5U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x25U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v166 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x26U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [6U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x26U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v167 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x27U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [7U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x27U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v168 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x28U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [8U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x28U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v169 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x29U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [9U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x29U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v170 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x2aU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xaU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x2aU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v171 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x2bU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xbU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x2bU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v172 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x2cU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xcU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x2cU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v173 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x2dU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xdU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x2dU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v174 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x2eU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xeU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x2eU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v175 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x2fU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xfU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x2fU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v176 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x30U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x30U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v177 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x31U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [1U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x31U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v178 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x32U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [2U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x32U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v179 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x33U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [3U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x33U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v180 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x34U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [4U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x34U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v181 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x35U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [5U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x35U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v182 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x36U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [6U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x36U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v183 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x37U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [7U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x37U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v184 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x38U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [8U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x38U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v185 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x39U] + (vlSelf->axi_sa16__DOT__A_buf
                                              [9U] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x39U])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v186 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x3aU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xaU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x3aU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v187 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x3bU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xbU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x3bU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v188 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x3cU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xcU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x3cU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v189 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x3dU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xdU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x3dU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v190 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x3eU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xeU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x3eU])));
                __Vdlyvval__axi_sa16__DOT__bypassC__v191 
                    = (0xfffffU & (vlSelf->axi_sa16__DOT__bypassC
                                   [0x3fU] + (vlSelf->axi_sa16__DOT__A_buf
                                              [0xfU] 
                                              * vlSelf->axi_sa16__DOT__B_buf
                                              [0x3fU])));
            }
        }
        if (((2U == (IData)(vlSelf->axi_sa16__DOT__sa_state)) 
             & (~ (IData)(vlSelf->axi_sa16__DOT__final_output_detected)))) {
            __Vdly__axi_sa16__DOT__compute_cycles = 
                (0x3ffU & ((IData)(1U) + (IData)(vlSelf->axi_sa16__DOT__compute_cycles)));
        }
        if (vlSelf->axi_sa16__DOT__w_hs) {
            if ((0x10U > (IData)(vlSelf->axi_sa16__DOT__load_count))) {
                if (vlSelf->axi_sa16__DOT__bank_load) {
                    __Vdlyvval__axi_sa16__DOT__inputA_bank1__v0 
                        = (0xffU & vlSelf->s_axi_wdata);
                    __Vdlyvset__axi_sa16__DOT__inputA_bank1__v0 = 1U;
                    __Vdlyvdim0__axi_sa16__DOT__inputA_bank1__v0 
                        = (0xfU & (IData)(vlSelf->axi_sa16__DOT__load_count));
                } else {
                    __Vdlyvval__axi_sa16__DOT__inputA_bank0__v0 
                        = (0xffU & vlSelf->s_axi_wdata);
                    __Vdlyvset__axi_sa16__DOT__inputA_bank0__v0 = 1U;
                    __Vdlyvdim0__axi_sa16__DOT__inputA_bank0__v0 
                        = (0xfU & (IData)(vlSelf->axi_sa16__DOT__load_count));
                }
                __Vdly__axi_sa16__DOT__load_count = 
                    (0x7fU & ((IData)(1U) + (IData)(vlSelf->axi_sa16__DOT__load_count)));
            } else if ((0x50U > (IData)(vlSelf->axi_sa16__DOT__load_count))) {
                if (vlSelf->axi_sa16__DOT__bank_load) {
                    __Vdlyvval__axi_sa16__DOT__inputB_bank1__v0 
                        = (0xffU & vlSelf->s_axi_wdata);
                    __Vdlyvset__axi_sa16__DOT__inputB_bank1__v0 = 1U;
                    __Vdlyvdim0__axi_sa16__DOT__inputB_bank1__v0 
                        = (0x3fU & ((IData)(vlSelf->axi_sa16__DOT__load_count) 
                                    - (IData)(0x10U)));
                } else {
                    __Vdlyvval__axi_sa16__DOT__inputB_bank0__v0 
                        = (0xffU & vlSelf->s_axi_wdata);
                    __Vdlyvset__axi_sa16__DOT__inputB_bank0__v0 = 1U;
                    __Vdlyvdim0__axi_sa16__DOT__inputB_bank0__v0 
                        = (0x3fU & ((IData)(vlSelf->axi_sa16__DOT__load_count) 
                                    - (IData)(0x10U)));
                }
                __Vdly__axi_sa16__DOT__load_count = 
                    (0x7fU & ((IData)(1U) + (IData)(vlSelf->axi_sa16__DOT__load_count)));
            }
            if ((0x4fU == (IData)(vlSelf->axi_sa16__DOT__load_count))) {
                if (vlSelf->axi_sa16__DOT__bank_load) {
                    __Vdly__axi_sa16__DOT__tile_ready1 = 1U;
                } else {
                    __Vdly__axi_sa16__DOT__tile_ready0 = 1U;
                }
                __Vdly__axi_sa16__DOT__load_count = 0U;
                __Vdly__axi_sa16__DOT__bank_load = 
                    (1U & (~ (IData)(vlSelf->axi_sa16__DOT__bank_load)));
            }
            if (vlSelf->axi_sa16__DOT__b_loaded) {
                if (vlSelf->axi_sa16__DOT__bank_load) {
                    __Vdlyvval__axi_sa16__DOT__inputA_bank1__v1 
                        = (0xffU & vlSelf->s_axi_wdata);
                    __Vdlyvset__axi_sa16__DOT__inputA_bank1__v1 = 1U;
                    __Vdlyvdim0__axi_sa16__DOT__inputA_bank1__v1 
                        = (0xfU & (IData)(vlSelf->axi_sa16__DOT__load_count));
                } else {
                    __Vdlyvval__axi_sa16__DOT__inputA_bank0__v1 
                        = (0xffU & vlSelf->s_axi_wdata);
                    __Vdlyvset__axi_sa16__DOT__inputA_bank0__v1 = 1U;
                    __Vdlyvdim0__axi_sa16__DOT__inputA_bank0__v1 
                        = (0xfU & (IData)(vlSelf->axi_sa16__DOT__load_count));
                }
                __Vdly__axi_sa16__DOT__load_count = 
                    (0x7fU & ((IData)(1U) + (IData)(vlSelf->axi_sa16__DOT__load_count)));
                if ((0xfU == (IData)(vlSelf->axi_sa16__DOT__load_count))) {
                    if (vlSelf->axi_sa16__DOT__bank_load) {
                        __Vdly__axi_sa16__DOT__tile_ready1 = 1U;
                    } else {
                        __Vdly__axi_sa16__DOT__tile_ready0 = 1U;
                    }
                    __Vdly__axi_sa16__DOT__load_count = 0U;
                    __Vdly__axi_sa16__DOT__bank_load 
                        = (1U & (~ (IData)(vlSelf->axi_sa16__DOT__bank_load)));
                }
            } else {
                __Vdlyvval__axi_sa16__DOT__inputB_bank0__v1 
                    = (0xffU & vlSelf->s_axi_wdata);
                __Vdlyvset__axi_sa16__DOT__inputB_bank0__v1 = 1U;
                __Vdlyvdim0__axi_sa16__DOT__inputB_bank0__v1 
                    = (0x3fU & (IData)(vlSelf->axi_sa16__DOT__load_count));
                __Vdlyvval__axi_sa16__DOT__inputB_bank1__v1 
                    = (0xffU & vlSelf->s_axi_wdata);
                __Vdlyvdim0__axi_sa16__DOT__inputB_bank1__v1 
                    = (0x3fU & (IData)(vlSelf->axi_sa16__DOT__load_count));
                __Vdly__axi_sa16__DOT__load_count = 
                    (0x7fU & ((IData)(1U) + (IData)(vlSelf->axi_sa16__DOT__load_count)));
                if ((0x3fU == (IData)(vlSelf->axi_sa16__DOT__load_count))) {
                    __Vdly__axi_sa16__DOT__b_loaded = 1U;
                    __Vdly__axi_sa16__DOT__load_count = 0U;
                }
            }
        }
    }
    if (vlSelf->rst) {
        __Vdly__axi_sa16__DOT__stable_cycles = 0U;
        __Vdly__axi_sa16__DOT__final_output_detected = 0U;
        __Vdlyvset__axi_sa16__DOT__prev_outputC__v0 = 1U;
    } else if (VL_UNLIKELY(vlSelf->axi_sa16__DOT__sa_start_pulse)) {
        VL_WRITEF("[%t] Tile compute start (bank %0#) clk=%0#\n",
                  64,VL_TIME_UNITED_Q(1),-12,1,(IData)(vlSelf->axi_sa16__DOT__bank_comp),
                  32,vlSelf->axi_sa16__DOT__counter);
        __Vdly__axi_sa16__DOT__stable_cycles = 0U;
        __Vdly__axi_sa16__DOT__final_output_detected = 0U;
        __Vdlyvset__axi_sa16__DOT__prev_outputC__v64 = 1U;
        VL_WRITEF("A_bank[%0d]= %0# %0# %0# %0# %0# %0# %0# %0#\n",
                  32,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                       ? 1U : 0U),8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                                      ? vlSelf->axi_sa16__DOT__inputA_bank1
                                     [0U] : vlSelf->axi_sa16__DOT__inputA_bank0
                                     [0U]),8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                                               ? vlSelf->axi_sa16__DOT__inputA_bank1
                                              [1U] : 
                                              vlSelf->axi_sa16__DOT__inputA_bank0
                                              [1U]),
                  8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                      ? vlSelf->axi_sa16__DOT__inputA_bank1
                     [2U] : vlSelf->axi_sa16__DOT__inputA_bank0
                     [2U]),8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                               ? vlSelf->axi_sa16__DOT__inputA_bank1
                              [3U] : vlSelf->axi_sa16__DOT__inputA_bank0
                              [3U]),8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                                        ? vlSelf->axi_sa16__DOT__inputA_bank1
                                       [4U] : vlSelf->axi_sa16__DOT__inputA_bank0
                                       [4U]),8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                                                 ? 
                                                vlSelf->axi_sa16__DOT__inputA_bank1
                                                [5U]
                                                 : 
                                                vlSelf->axi_sa16__DOT__inputA_bank0
                                                [5U]),
                  8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                      ? vlSelf->axi_sa16__DOT__inputA_bank1
                     [6U] : vlSelf->axi_sa16__DOT__inputA_bank0
                     [6U]),8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                               ? vlSelf->axi_sa16__DOT__inputA_bank1
                              [7U] : vlSelf->axi_sa16__DOT__inputA_bank0
                              [7U]));
        __Vdlyvset__axi_sa16__DOT__prev_outputC__v65 = 1U;
        VL_WRITEF("B_bank[%0d]= %0# %0# %0# %0# %0# %0# %0# %0#\n",
                  32,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                       ? 1U : 0U),8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                                      ? vlSelf->axi_sa16__DOT__inputB_bank1
                                     [0U] : vlSelf->axi_sa16__DOT__inputB_bank0
                                     [0U]),8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                                               ? vlSelf->axi_sa16__DOT__inputB_bank1
                                              [1U] : 
                                              vlSelf->axi_sa16__DOT__inputB_bank0
                                              [1U]),
                  8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                      ? vlSelf->axi_sa16__DOT__inputB_bank1
                     [2U] : vlSelf->axi_sa16__DOT__inputB_bank0
                     [2U]),8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                               ? vlSelf->axi_sa16__DOT__inputB_bank1
                              [3U] : vlSelf->axi_sa16__DOT__inputB_bank0
                              [3U]),8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                                        ? vlSelf->axi_sa16__DOT__inputB_bank1
                                       [4U] : vlSelf->axi_sa16__DOT__inputB_bank0
                                       [4U]),8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                                                 ? 
                                                vlSelf->axi_sa16__DOT__inputB_bank1
                                                [5U]
                                                 : 
                                                vlSelf->axi_sa16__DOT__inputB_bank0
                                                [5U]),
                  8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                      ? vlSelf->axi_sa16__DOT__inputB_bank1
                     [6U] : vlSelf->axi_sa16__DOT__inputB_bank0
                     [6U]),8,((IData)(vlSelf->axi_sa16__DOT__bank_comp)
                               ? vlSelf->axi_sa16__DOT__inputB_bank1
                              [7U] : vlSelf->axi_sa16__DOT__inputB_bank0
                              [7U]));
        __Vdlyvset__axi_sa16__DOT__prev_outputC__v66 = 1U;
    } else if (((((IData)(vlSelf->axi_sa16__DOT__sa_busy) 
                  & (~ (IData)(vlSelf->axi_sa16__DOT__final_output_detected))) 
                 & (0U == (IData)(vlSelf->axi_sa16__DOT__warmup_cycles))) 
                & ((IData)(vlSelf->axi_sa16__DOT__compute_cycles) 
                   >= (IData)(vlSelf->axi_sa16__DOT__compute_cycles_req)))) {
        vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt = 0U;
        if ((vlSelf->axi_sa16__DOT__outputC[0U] == 
             vlSelf->axi_sa16__DOT__prev_outputC[0U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[1U] == 
             vlSelf->axi_sa16__DOT__prev_outputC[1U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[2U] == 
             vlSelf->axi_sa16__DOT__prev_outputC[2U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[3U] == 
             vlSelf->axi_sa16__DOT__prev_outputC[3U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[4U] == 
             vlSelf->axi_sa16__DOT__prev_outputC[4U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[5U] == 
             vlSelf->axi_sa16__DOT__prev_outputC[5U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[6U] == 
             vlSelf->axi_sa16__DOT__prev_outputC[6U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[7U] == 
             vlSelf->axi_sa16__DOT__prev_outputC[7U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[8U] == 
             vlSelf->axi_sa16__DOT__prev_outputC[8U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[9U] == 
             vlSelf->axi_sa16__DOT__prev_outputC[9U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0xaU] == 
             vlSelf->axi_sa16__DOT__prev_outputC[0xaU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0xbU] == 
             vlSelf->axi_sa16__DOT__prev_outputC[0xbU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0xcU] == 
             vlSelf->axi_sa16__DOT__prev_outputC[0xcU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0xdU] == 
             vlSelf->axi_sa16__DOT__prev_outputC[0xdU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0xeU] == 
             vlSelf->axi_sa16__DOT__prev_outputC[0xeU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0xfU] == 
             vlSelf->axi_sa16__DOT__prev_outputC[0xfU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x10U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x10U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x11U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x11U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x12U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x12U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x13U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x13U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x14U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x14U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x15U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x15U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x16U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x16U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x17U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x17U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x18U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x18U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x19U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x19U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x1aU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x1aU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x1bU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x1bU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x1cU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x1cU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x1dU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x1dU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x1eU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x1eU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x1fU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x1fU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x20U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x20U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x21U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x21U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x22U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x22U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x23U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x23U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x24U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x24U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x25U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x25U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x26U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x26U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x27U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x27U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x28U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x28U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x29U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x29U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x2aU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x2aU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x2bU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x2bU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x2cU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x2cU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x2dU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x2dU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x2eU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x2eU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x2fU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x2fU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x30U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x30U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x31U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x31U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x32U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x32U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x33U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x33U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x34U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x34U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x35U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x35U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x36U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x36U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x37U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x37U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x38U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x38U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x39U] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x39U])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x3aU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x3aU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x3bU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x3bU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x3cU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x3cU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x3dU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x3dU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x3eU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x3eU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        if ((vlSelf->axi_sa16__DOT__outputC[0x3fU] 
             == vlSelf->axi_sa16__DOT__prev_outputC
             [0x3fU])) {
            vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt 
                = ((IData)(1U) + vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt);
        }
        __Vdly__axi_sa16__DOT__stable_cycles = ((0x40U 
                                                 == vlSelf->axi_sa16__DOT__unnamedblk3__DOT__eq_cnt)
                                                 ? 
                                                (0x3fU 
                                                 & ((IData)(1U) 
                                                    + (IData)(vlSelf->axi_sa16__DOT__stable_cycles)))
                                                 : 0U);
        if (VL_UNLIKELY((5U <= (IData)(vlSelf->axi_sa16__DOT__stable_cycles)))) {
            VL_WRITEF("[%t] Tile stabilized (bank %0#) clk=%0#\n",
                      64,VL_TIME_UNITED_Q(1),-12,1,
                      (IData)(vlSelf->axi_sa16__DOT__bank_comp),
                      32,vlSelf->axi_sa16__DOT__counter);
            __Vdly__axi_sa16__DOT__final_output_detected = 1U;
        }
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v128 
            = vlSelf->axi_sa16__DOT__outputC[0U];
        __Vdlyvset__axi_sa16__DOT__prev_outputC__v128 = 1U;
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v129 
            = vlSelf->axi_sa16__DOT__outputC[1U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v130 
            = vlSelf->axi_sa16__DOT__outputC[2U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v131 
            = vlSelf->axi_sa16__DOT__outputC[3U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v132 
            = vlSelf->axi_sa16__DOT__outputC[4U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v133 
            = vlSelf->axi_sa16__DOT__outputC[5U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v134 
            = vlSelf->axi_sa16__DOT__outputC[6U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v135 
            = vlSelf->axi_sa16__DOT__outputC[7U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v136 
            = vlSelf->axi_sa16__DOT__outputC[8U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v137 
            = vlSelf->axi_sa16__DOT__outputC[9U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v138 
            = vlSelf->axi_sa16__DOT__outputC[0xaU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v139 
            = vlSelf->axi_sa16__DOT__outputC[0xbU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v140 
            = vlSelf->axi_sa16__DOT__outputC[0xcU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v141 
            = vlSelf->axi_sa16__DOT__outputC[0xdU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v142 
            = vlSelf->axi_sa16__DOT__outputC[0xeU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v143 
            = vlSelf->axi_sa16__DOT__outputC[0xfU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v144 
            = vlSelf->axi_sa16__DOT__outputC[0x10U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v145 
            = vlSelf->axi_sa16__DOT__outputC[0x11U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v146 
            = vlSelf->axi_sa16__DOT__outputC[0x12U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v147 
            = vlSelf->axi_sa16__DOT__outputC[0x13U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v148 
            = vlSelf->axi_sa16__DOT__outputC[0x14U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v149 
            = vlSelf->axi_sa16__DOT__outputC[0x15U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v150 
            = vlSelf->axi_sa16__DOT__outputC[0x16U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v151 
            = vlSelf->axi_sa16__DOT__outputC[0x17U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v152 
            = vlSelf->axi_sa16__DOT__outputC[0x18U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v153 
            = vlSelf->axi_sa16__DOT__outputC[0x19U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v154 
            = vlSelf->axi_sa16__DOT__outputC[0x1aU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v155 
            = vlSelf->axi_sa16__DOT__outputC[0x1bU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v156 
            = vlSelf->axi_sa16__DOT__outputC[0x1cU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v157 
            = vlSelf->axi_sa16__DOT__outputC[0x1dU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v158 
            = vlSelf->axi_sa16__DOT__outputC[0x1eU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v159 
            = vlSelf->axi_sa16__DOT__outputC[0x1fU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v160 
            = vlSelf->axi_sa16__DOT__outputC[0x20U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v161 
            = vlSelf->axi_sa16__DOT__outputC[0x21U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v162 
            = vlSelf->axi_sa16__DOT__outputC[0x22U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v163 
            = vlSelf->axi_sa16__DOT__outputC[0x23U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v164 
            = vlSelf->axi_sa16__DOT__outputC[0x24U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v165 
            = vlSelf->axi_sa16__DOT__outputC[0x25U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v166 
            = vlSelf->axi_sa16__DOT__outputC[0x26U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v167 
            = vlSelf->axi_sa16__DOT__outputC[0x27U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v168 
            = vlSelf->axi_sa16__DOT__outputC[0x28U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v169 
            = vlSelf->axi_sa16__DOT__outputC[0x29U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v170 
            = vlSelf->axi_sa16__DOT__outputC[0x2aU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v171 
            = vlSelf->axi_sa16__DOT__outputC[0x2bU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v172 
            = vlSelf->axi_sa16__DOT__outputC[0x2cU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v173 
            = vlSelf->axi_sa16__DOT__outputC[0x2dU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v174 
            = vlSelf->axi_sa16__DOT__outputC[0x2eU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v175 
            = vlSelf->axi_sa16__DOT__outputC[0x2fU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v176 
            = vlSelf->axi_sa16__DOT__outputC[0x30U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v177 
            = vlSelf->axi_sa16__DOT__outputC[0x31U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v178 
            = vlSelf->axi_sa16__DOT__outputC[0x32U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v179 
            = vlSelf->axi_sa16__DOT__outputC[0x33U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v180 
            = vlSelf->axi_sa16__DOT__outputC[0x34U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v181 
            = vlSelf->axi_sa16__DOT__outputC[0x35U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v182 
            = vlSelf->axi_sa16__DOT__outputC[0x36U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v183 
            = vlSelf->axi_sa16__DOT__outputC[0x37U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v184 
            = vlSelf->axi_sa16__DOT__outputC[0x38U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v185 
            = vlSelf->axi_sa16__DOT__outputC[0x39U];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v186 
            = vlSelf->axi_sa16__DOT__outputC[0x3aU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v187 
            = vlSelf->axi_sa16__DOT__outputC[0x3bU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v188 
            = vlSelf->axi_sa16__DOT__outputC[0x3cU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v189 
            = vlSelf->axi_sa16__DOT__outputC[0x3dU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v190 
            = vlSelf->axi_sa16__DOT__outputC[0x3eU];
        __Vdlyvval__axi_sa16__DOT__prev_outputC__v191 
            = vlSelf->axi_sa16__DOT__outputC[0x3fU];
    }
    if (vlSelf->rst) {
        vlSelf->axi_sa16__DOT__compute_cycles_req = 0x80U;
        __Vdly__axi_sa16__DOT__sa_state = 0U;
        __Vdly__axi_sa16__DOT__bank_comp = 1U;
        vlSelf->axi_sa16__DOT__sa_busy = 0U;
        vlSelf->axi_sa16__DOT__sa_start_pulse = 0U;
    } else {
        vlSelf->axi_sa16__DOT__sa_start_pulse = 0U;
        if ((0U == (IData)(vlSelf->axi_sa16__DOT__sa_state))) {
            vlSelf->axi_sa16__DOT__sa_busy = 0U;
            if ((((~ (IData)(vlSelf->axi_sa16__DOT__bank_comp)) 
                  & (IData)(vlSelf->axi_sa16__DOT__tile_ready0)) 
                 | ((IData)(vlSelf->axi_sa16__DOT__bank_comp) 
                    & (IData)(vlSelf->axi_sa16__DOT__tile_ready1)))) {
                if (vlSelf->axi_sa16__DOT__bank_comp) {
                    __Vdlyvval__axi_sa16__DOT__A_buf__v0 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [0U];
                    __Vdlyvset__axi_sa16__DOT__A_buf__v0 = 1U;
                    __Vdly__axi_sa16__DOT__tile_ready1 = 0U;
                    __Vdlyvval__axi_sa16__DOT__A_buf__v1 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [1U];
                    __Vdlyvset__axi_sa16__DOT__A_buf__v1 = 1U;
                    __Vdlyvval__axi_sa16__DOT__A_buf__v2 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [2U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v3 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [3U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v4 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [4U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v5 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [5U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v6 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [6U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v7 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [7U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v8 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [8U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v9 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [9U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v10 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [0xaU];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v11 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [0xbU];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v12 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [0xcU];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v13 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [0xdU];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v14 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [0xeU];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v15 
                        = vlSelf->axi_sa16__DOT__inputA_bank1
                        [0xfU];
                } else {
                    __Vdlyvval__axi_sa16__DOT__A_buf__v16 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [0U];
                    __Vdlyvset__axi_sa16__DOT__A_buf__v16 = 1U;
                    __Vdly__axi_sa16__DOT__tile_ready0 = 0U;
                    __Vdlyvval__axi_sa16__DOT__A_buf__v17 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [1U];
                    __Vdlyvset__axi_sa16__DOT__A_buf__v17 = 1U;
                    __Vdlyvval__axi_sa16__DOT__A_buf__v18 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [2U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v19 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [3U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v20 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [4U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v21 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [5U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v22 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [6U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v23 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [7U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v24 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [8U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v25 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [9U];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v26 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [0xaU];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v27 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [0xbU];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v28 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [0xcU];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v29 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [0xdU];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v30 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [0xeU];
                    __Vdlyvval__axi_sa16__DOT__A_buf__v31 
                        = vlSelf->axi_sa16__DOT__inputA_bank0
                        [0xfU];
                }
                __Vdlyvval__axi_sa16__DOT__B_buf__v0 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0U];
                __Vdlyvset__axi_sa16__DOT__B_buf__v0 = 1U;
                __Vdly__axi_sa16__DOT__sa_state = 1U;
                __Vdlyvval__axi_sa16__DOT__B_buf__v1 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [1U];
                __Vdlyvset__axi_sa16__DOT__B_buf__v1 = 1U;
                __Vdlyvval__axi_sa16__DOT__B_buf__v2 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [2U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v3 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [3U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v4 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [4U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v5 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [5U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v6 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [6U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v7 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [7U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v8 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [8U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v9 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [9U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v10 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0xaU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v11 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0xbU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v12 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0xcU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v13 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0xdU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v14 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0xeU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v15 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0xfU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v16 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x10U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v17 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x11U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v18 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x12U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v19 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x13U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v20 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x14U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v21 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x15U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v22 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x16U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v23 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x17U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v24 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x18U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v25 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x19U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v26 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x1aU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v27 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x1bU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v28 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x1cU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v29 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x1dU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v30 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x1eU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v31 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x1fU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v32 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x20U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v33 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x21U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v34 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x22U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v35 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x23U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v36 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x24U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v37 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x25U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v38 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x26U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v39 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x27U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v40 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x28U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v41 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x29U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v42 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x2aU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v43 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x2bU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v44 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x2cU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v45 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x2dU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v46 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x2eU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v47 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x2fU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v48 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x30U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v49 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x31U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v50 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x32U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v51 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x33U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v52 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x34U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v53 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x35U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v54 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x36U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v55 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x37U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v56 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x38U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v57 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x39U];
                __Vdlyvval__axi_sa16__DOT__B_buf__v58 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x3aU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v59 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x3bU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v60 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x3cU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v61 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x3dU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v62 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x3eU];
                __Vdlyvval__axi_sa16__DOT__B_buf__v63 
                    = vlSelf->axi_sa16__DOT__inputB_bank0
                    [0x3fU];
            } else if ((((IData)(vlSelf->axi_sa16__DOT__bank_comp) 
                         & (IData)(vlSelf->axi_sa16__DOT__tile_ready0)) 
                        | ((~ (IData)(vlSelf->axi_sa16__DOT__bank_comp)) 
                           & (IData)(vlSelf->axi_sa16__DOT__tile_ready1)))) {
                __Vdly__axi_sa16__DOT__bank_comp = 
                    (1U & (~ (IData)(vlSelf->axi_sa16__DOT__bank_comp)));
            }
        } else if ((1U == (IData)(vlSelf->axi_sa16__DOT__sa_state))) {
            vlSelf->axi_sa16__DOT__sa_start_pulse = 1U;
            vlSelf->axi_sa16__DOT__sa_busy = 1U;
            __Vdly__axi_sa16__DOT__sa_state = 2U;
        } else if ((2U == (IData)(vlSelf->axi_sa16__DOT__sa_state))) {
            if (vlSelf->axi_sa16__DOT__final_output_detected) {
                __Vdly__axi_sa16__DOT__sa_state = 0U;
            }
        }
    }
    vlSelf->axi_sa16__DOT__print_pending = __Vdly__axi_sa16__DOT__print_pending;
    if (__Vdlyvset__axi_sa16__DOT__output_mem__v0) {
        __Vilp = 0U;
        while ((__Vilp <= 0x3fU)) {
            vlSelf->axi_sa16__DOT__output_mem[__Vilp] = 0U;
            __Vilp = ((IData)(1U) + __Vilp);
        }
    }
    if (__Vdlyvset__axi_sa16__DOT__output_mem__v64) {
        vlSelf->axi_sa16__DOT__output_mem[0U] = __Vdlyvval__axi_sa16__DOT__output_mem__v64;
        vlSelf->axi_sa16__DOT__output_mem[1U] = __Vdlyvval__axi_sa16__DOT__output_mem__v65;
        vlSelf->axi_sa16__DOT__output_mem[2U] = __Vdlyvval__axi_sa16__DOT__output_mem__v66;
        vlSelf->axi_sa16__DOT__output_mem[3U] = __Vdlyvval__axi_sa16__DOT__output_mem__v67;
        vlSelf->axi_sa16__DOT__output_mem[4U] = __Vdlyvval__axi_sa16__DOT__output_mem__v68;
        vlSelf->axi_sa16__DOT__output_mem[5U] = __Vdlyvval__axi_sa16__DOT__output_mem__v69;
        vlSelf->axi_sa16__DOT__output_mem[6U] = __Vdlyvval__axi_sa16__DOT__output_mem__v70;
        vlSelf->axi_sa16__DOT__output_mem[7U] = __Vdlyvval__axi_sa16__DOT__output_mem__v71;
        vlSelf->axi_sa16__DOT__output_mem[8U] = __Vdlyvval__axi_sa16__DOT__output_mem__v72;
        vlSelf->axi_sa16__DOT__output_mem[9U] = __Vdlyvval__axi_sa16__DOT__output_mem__v73;
        vlSelf->axi_sa16__DOT__output_mem[0xaU] = __Vdlyvval__axi_sa16__DOT__output_mem__v74;
        vlSelf->axi_sa16__DOT__output_mem[0xbU] = __Vdlyvval__axi_sa16__DOT__output_mem__v75;
        vlSelf->axi_sa16__DOT__output_mem[0xcU] = __Vdlyvval__axi_sa16__DOT__output_mem__v76;
        vlSelf->axi_sa16__DOT__output_mem[0xdU] = __Vdlyvval__axi_sa16__DOT__output_mem__v77;
        vlSelf->axi_sa16__DOT__output_mem[0xeU] = __Vdlyvval__axi_sa16__DOT__output_mem__v78;
        vlSelf->axi_sa16__DOT__output_mem[0xfU] = __Vdlyvval__axi_sa16__DOT__output_mem__v79;
        vlSelf->axi_sa16__DOT__output_mem[0x10U] = __Vdlyvval__axi_sa16__DOT__output_mem__v80;
        vlSelf->axi_sa16__DOT__output_mem[0x11U] = __Vdlyvval__axi_sa16__DOT__output_mem__v81;
        vlSelf->axi_sa16__DOT__output_mem[0x12U] = __Vdlyvval__axi_sa16__DOT__output_mem__v82;
        vlSelf->axi_sa16__DOT__output_mem[0x13U] = __Vdlyvval__axi_sa16__DOT__output_mem__v83;
        vlSelf->axi_sa16__DOT__output_mem[0x14U] = __Vdlyvval__axi_sa16__DOT__output_mem__v84;
        vlSelf->axi_sa16__DOT__output_mem[0x15U] = __Vdlyvval__axi_sa16__DOT__output_mem__v85;
        vlSelf->axi_sa16__DOT__output_mem[0x16U] = __Vdlyvval__axi_sa16__DOT__output_mem__v86;
        vlSelf->axi_sa16__DOT__output_mem[0x17U] = __Vdlyvval__axi_sa16__DOT__output_mem__v87;
        vlSelf->axi_sa16__DOT__output_mem[0x18U] = __Vdlyvval__axi_sa16__DOT__output_mem__v88;
        vlSelf->axi_sa16__DOT__output_mem[0x19U] = __Vdlyvval__axi_sa16__DOT__output_mem__v89;
        vlSelf->axi_sa16__DOT__output_mem[0x1aU] = __Vdlyvval__axi_sa16__DOT__output_mem__v90;
        vlSelf->axi_sa16__DOT__output_mem[0x1bU] = __Vdlyvval__axi_sa16__DOT__output_mem__v91;
        vlSelf->axi_sa16__DOT__output_mem[0x1cU] = __Vdlyvval__axi_sa16__DOT__output_mem__v92;
        vlSelf->axi_sa16__DOT__output_mem[0x1dU] = __Vdlyvval__axi_sa16__DOT__output_mem__v93;
        vlSelf->axi_sa16__DOT__output_mem[0x1eU] = __Vdlyvval__axi_sa16__DOT__output_mem__v94;
        vlSelf->axi_sa16__DOT__output_mem[0x1fU] = __Vdlyvval__axi_sa16__DOT__output_mem__v95;
        vlSelf->axi_sa16__DOT__output_mem[0x20U] = __Vdlyvval__axi_sa16__DOT__output_mem__v96;
        vlSelf->axi_sa16__DOT__output_mem[0x21U] = __Vdlyvval__axi_sa16__DOT__output_mem__v97;
        vlSelf->axi_sa16__DOT__output_mem[0x22U] = __Vdlyvval__axi_sa16__DOT__output_mem__v98;
        vlSelf->axi_sa16__DOT__output_mem[0x23U] = __Vdlyvval__axi_sa16__DOT__output_mem__v99;
        vlSelf->axi_sa16__DOT__output_mem[0x24U] = __Vdlyvval__axi_sa16__DOT__output_mem__v100;
        vlSelf->axi_sa16__DOT__output_mem[0x25U] = __Vdlyvval__axi_sa16__DOT__output_mem__v101;
        vlSelf->axi_sa16__DOT__output_mem[0x26U] = __Vdlyvval__axi_sa16__DOT__output_mem__v102;
        vlSelf->axi_sa16__DOT__output_mem[0x27U] = __Vdlyvval__axi_sa16__DOT__output_mem__v103;
        vlSelf->axi_sa16__DOT__output_mem[0x28U] = __Vdlyvval__axi_sa16__DOT__output_mem__v104;
        vlSelf->axi_sa16__DOT__output_mem[0x29U] = __Vdlyvval__axi_sa16__DOT__output_mem__v105;
        vlSelf->axi_sa16__DOT__output_mem[0x2aU] = __Vdlyvval__axi_sa16__DOT__output_mem__v106;
        vlSelf->axi_sa16__DOT__output_mem[0x2bU] = __Vdlyvval__axi_sa16__DOT__output_mem__v107;
        vlSelf->axi_sa16__DOT__output_mem[0x2cU] = __Vdlyvval__axi_sa16__DOT__output_mem__v108;
        vlSelf->axi_sa16__DOT__output_mem[0x2dU] = __Vdlyvval__axi_sa16__DOT__output_mem__v109;
        vlSelf->axi_sa16__DOT__output_mem[0x2eU] = __Vdlyvval__axi_sa16__DOT__output_mem__v110;
        vlSelf->axi_sa16__DOT__output_mem[0x2fU] = __Vdlyvval__axi_sa16__DOT__output_mem__v111;
        vlSelf->axi_sa16__DOT__output_mem[0x30U] = __Vdlyvval__axi_sa16__DOT__output_mem__v112;
        vlSelf->axi_sa16__DOT__output_mem[0x31U] = __Vdlyvval__axi_sa16__DOT__output_mem__v113;
        vlSelf->axi_sa16__DOT__output_mem[0x32U] = __Vdlyvval__axi_sa16__DOT__output_mem__v114;
        vlSelf->axi_sa16__DOT__output_mem[0x33U] = __Vdlyvval__axi_sa16__DOT__output_mem__v115;
        vlSelf->axi_sa16__DOT__output_mem[0x34U] = __Vdlyvval__axi_sa16__DOT__output_mem__v116;
        vlSelf->axi_sa16__DOT__output_mem[0x35U] = __Vdlyvval__axi_sa16__DOT__output_mem__v117;
        vlSelf->axi_sa16__DOT__output_mem[0x36U] = __Vdlyvval__axi_sa16__DOT__output_mem__v118;
        vlSelf->axi_sa16__DOT__output_mem[0x37U] = __Vdlyvval__axi_sa16__DOT__output_mem__v119;
        vlSelf->axi_sa16__DOT__output_mem[0x38U] = __Vdlyvval__axi_sa16__DOT__output_mem__v120;
        vlSelf->axi_sa16__DOT__output_mem[0x39U] = __Vdlyvval__axi_sa16__DOT__output_mem__v121;
        vlSelf->axi_sa16__DOT__output_mem[0x3aU] = __Vdlyvval__axi_sa16__DOT__output_mem__v122;
        vlSelf->axi_sa16__DOT__output_mem[0x3bU] = __Vdlyvval__axi_sa16__DOT__output_mem__v123;
        vlSelf->axi_sa16__DOT__output_mem[0x3cU] = __Vdlyvval__axi_sa16__DOT__output_mem__v124;
        vlSelf->axi_sa16__DOT__output_mem[0x3dU] = __Vdlyvval__axi_sa16__DOT__output_mem__v125;
        vlSelf->axi_sa16__DOT__output_mem[0x3eU] = __Vdlyvval__axi_sa16__DOT__output_mem__v126;
        vlSelf->axi_sa16__DOT__output_mem[0x3fU] = __Vdlyvval__axi_sa16__DOT__output_mem__v127;
    }
    vlSelf->axi_sa16__DOT__b_loaded = __Vdly__axi_sa16__DOT__b_loaded;
    vlSelf->axi_sa16__DOT__bank_load = __Vdly__axi_sa16__DOT__bank_load;
    vlSelf->axi_sa16__DOT__load_count = __Vdly__axi_sa16__DOT__load_count;
    if (__Vdlyvset__axi_sa16__DOT__bypassC__v0) {
        __Vilp = 0U;
        while ((__Vilp <= 0x3fU)) {
            vlSelf->axi_sa16__DOT__bypassC[__Vilp] = 0U;
            __Vilp = ((IData)(1U) + __Vilp);
        }
    }
    if (__Vdlyvset__axi_sa16__DOT__bypassC__v64) {
        __Vilp = 0U;
        while ((__Vilp <= 0x3fU)) {
            vlSelf->axi_sa16__DOT__bypassC[__Vilp] = 0U;
            __Vilp = ((IData)(1U) + __Vilp);
        }
    }
    if (__Vdlyvset__axi_sa16__DOT__bypassC__v128) {
        vlSelf->axi_sa16__DOT__bypassC[0U] = __Vdlyvval__axi_sa16__DOT__bypassC__v128;
        vlSelf->axi_sa16__DOT__bypassC[1U] = __Vdlyvval__axi_sa16__DOT__bypassC__v129;
        vlSelf->axi_sa16__DOT__bypassC[2U] = __Vdlyvval__axi_sa16__DOT__bypassC__v130;
        vlSelf->axi_sa16__DOT__bypassC[3U] = __Vdlyvval__axi_sa16__DOT__bypassC__v131;
        vlSelf->axi_sa16__DOT__bypassC[4U] = __Vdlyvval__axi_sa16__DOT__bypassC__v132;
        vlSelf->axi_sa16__DOT__bypassC[5U] = __Vdlyvval__axi_sa16__DOT__bypassC__v133;
        vlSelf->axi_sa16__DOT__bypassC[6U] = __Vdlyvval__axi_sa16__DOT__bypassC__v134;
        vlSelf->axi_sa16__DOT__bypassC[7U] = __Vdlyvval__axi_sa16__DOT__bypassC__v135;
        vlSelf->axi_sa16__DOT__bypassC[8U] = __Vdlyvval__axi_sa16__DOT__bypassC__v136;
        vlSelf->axi_sa16__DOT__bypassC[9U] = __Vdlyvval__axi_sa16__DOT__bypassC__v137;
        vlSelf->axi_sa16__DOT__bypassC[0xaU] = __Vdlyvval__axi_sa16__DOT__bypassC__v138;
        vlSelf->axi_sa16__DOT__bypassC[0xbU] = __Vdlyvval__axi_sa16__DOT__bypassC__v139;
        vlSelf->axi_sa16__DOT__bypassC[0xcU] = __Vdlyvval__axi_sa16__DOT__bypassC__v140;
        vlSelf->axi_sa16__DOT__bypassC[0xdU] = __Vdlyvval__axi_sa16__DOT__bypassC__v141;
        vlSelf->axi_sa16__DOT__bypassC[0xeU] = __Vdlyvval__axi_sa16__DOT__bypassC__v142;
        vlSelf->axi_sa16__DOT__bypassC[0xfU] = __Vdlyvval__axi_sa16__DOT__bypassC__v143;
        vlSelf->axi_sa16__DOT__bypassC[0x10U] = __Vdlyvval__axi_sa16__DOT__bypassC__v144;
        vlSelf->axi_sa16__DOT__bypassC[0x11U] = __Vdlyvval__axi_sa16__DOT__bypassC__v145;
        vlSelf->axi_sa16__DOT__bypassC[0x12U] = __Vdlyvval__axi_sa16__DOT__bypassC__v146;
        vlSelf->axi_sa16__DOT__bypassC[0x13U] = __Vdlyvval__axi_sa16__DOT__bypassC__v147;
        vlSelf->axi_sa16__DOT__bypassC[0x14U] = __Vdlyvval__axi_sa16__DOT__bypassC__v148;
        vlSelf->axi_sa16__DOT__bypassC[0x15U] = __Vdlyvval__axi_sa16__DOT__bypassC__v149;
        vlSelf->axi_sa16__DOT__bypassC[0x16U] = __Vdlyvval__axi_sa16__DOT__bypassC__v150;
        vlSelf->axi_sa16__DOT__bypassC[0x17U] = __Vdlyvval__axi_sa16__DOT__bypassC__v151;
        vlSelf->axi_sa16__DOT__bypassC[0x18U] = __Vdlyvval__axi_sa16__DOT__bypassC__v152;
        vlSelf->axi_sa16__DOT__bypassC[0x19U] = __Vdlyvval__axi_sa16__DOT__bypassC__v153;
        vlSelf->axi_sa16__DOT__bypassC[0x1aU] = __Vdlyvval__axi_sa16__DOT__bypassC__v154;
        vlSelf->axi_sa16__DOT__bypassC[0x1bU] = __Vdlyvval__axi_sa16__DOT__bypassC__v155;
        vlSelf->axi_sa16__DOT__bypassC[0x1cU] = __Vdlyvval__axi_sa16__DOT__bypassC__v156;
        vlSelf->axi_sa16__DOT__bypassC[0x1dU] = __Vdlyvval__axi_sa16__DOT__bypassC__v157;
        vlSelf->axi_sa16__DOT__bypassC[0x1eU] = __Vdlyvval__axi_sa16__DOT__bypassC__v158;
        vlSelf->axi_sa16__DOT__bypassC[0x1fU] = __Vdlyvval__axi_sa16__DOT__bypassC__v159;
        vlSelf->axi_sa16__DOT__bypassC[0x20U] = __Vdlyvval__axi_sa16__DOT__bypassC__v160;
        vlSelf->axi_sa16__DOT__bypassC[0x21U] = __Vdlyvval__axi_sa16__DOT__bypassC__v161;
        vlSelf->axi_sa16__DOT__bypassC[0x22U] = __Vdlyvval__axi_sa16__DOT__bypassC__v162;
        vlSelf->axi_sa16__DOT__bypassC[0x23U] = __Vdlyvval__axi_sa16__DOT__bypassC__v163;
        vlSelf->axi_sa16__DOT__bypassC[0x24U] = __Vdlyvval__axi_sa16__DOT__bypassC__v164;
        vlSelf->axi_sa16__DOT__bypassC[0x25U] = __Vdlyvval__axi_sa16__DOT__bypassC__v165;
        vlSelf->axi_sa16__DOT__bypassC[0x26U] = __Vdlyvval__axi_sa16__DOT__bypassC__v166;
        vlSelf->axi_sa16__DOT__bypassC[0x27U] = __Vdlyvval__axi_sa16__DOT__bypassC__v167;
        vlSelf->axi_sa16__DOT__bypassC[0x28U] = __Vdlyvval__axi_sa16__DOT__bypassC__v168;
        vlSelf->axi_sa16__DOT__bypassC[0x29U] = __Vdlyvval__axi_sa16__DOT__bypassC__v169;
        vlSelf->axi_sa16__DOT__bypassC[0x2aU] = __Vdlyvval__axi_sa16__DOT__bypassC__v170;
        vlSelf->axi_sa16__DOT__bypassC[0x2bU] = __Vdlyvval__axi_sa16__DOT__bypassC__v171;
        vlSelf->axi_sa16__DOT__bypassC[0x2cU] = __Vdlyvval__axi_sa16__DOT__bypassC__v172;
        vlSelf->axi_sa16__DOT__bypassC[0x2dU] = __Vdlyvval__axi_sa16__DOT__bypassC__v173;
        vlSelf->axi_sa16__DOT__bypassC[0x2eU] = __Vdlyvval__axi_sa16__DOT__bypassC__v174;
        vlSelf->axi_sa16__DOT__bypassC[0x2fU] = __Vdlyvval__axi_sa16__DOT__bypassC__v175;
        vlSelf->axi_sa16__DOT__bypassC[0x30U] = __Vdlyvval__axi_sa16__DOT__bypassC__v176;
        vlSelf->axi_sa16__DOT__bypassC[0x31U] = __Vdlyvval__axi_sa16__DOT__bypassC__v177;
        vlSelf->axi_sa16__DOT__bypassC[0x32U] = __Vdlyvval__axi_sa16__DOT__bypassC__v178;
        vlSelf->axi_sa16__DOT__bypassC[0x33U] = __Vdlyvval__axi_sa16__DOT__bypassC__v179;
        vlSelf->axi_sa16__DOT__bypassC[0x34U] = __Vdlyvval__axi_sa16__DOT__bypassC__v180;
        vlSelf->axi_sa16__DOT__bypassC[0x35U] = __Vdlyvval__axi_sa16__DOT__bypassC__v181;
        vlSelf->axi_sa16__DOT__bypassC[0x36U] = __Vdlyvval__axi_sa16__DOT__bypassC__v182;
        vlSelf->axi_sa16__DOT__bypassC[0x37U] = __Vdlyvval__axi_sa16__DOT__bypassC__v183;
        vlSelf->axi_sa16__DOT__bypassC[0x38U] = __Vdlyvval__axi_sa16__DOT__bypassC__v184;
        vlSelf->axi_sa16__DOT__bypassC[0x39U] = __Vdlyvval__axi_sa16__DOT__bypassC__v185;
        vlSelf->axi_sa16__DOT__bypassC[0x3aU] = __Vdlyvval__axi_sa16__DOT__bypassC__v186;
        vlSelf->axi_sa16__DOT__bypassC[0x3bU] = __Vdlyvval__axi_sa16__DOT__bypassC__v187;
        vlSelf->axi_sa16__DOT__bypassC[0x3cU] = __Vdlyvval__axi_sa16__DOT__bypassC__v188;
        vlSelf->axi_sa16__DOT__bypassC[0x3dU] = __Vdlyvval__axi_sa16__DOT__bypassC__v189;
        vlSelf->axi_sa16__DOT__bypassC[0x3eU] = __Vdlyvval__axi_sa16__DOT__bypassC__v190;
        vlSelf->axi_sa16__DOT__bypassC[0x3fU] = __Vdlyvval__axi_sa16__DOT__bypassC__v191;
    }
    vlSelf->axi_sa16__DOT__stable_cycles = __Vdly__axi_sa16__DOT__stable_cycles;
    vlSelf->axi_sa16__DOT__counter = __Vdly__axi_sa16__DOT__counter;
    vlSelf->axi_sa16__DOT__compute_cycles = __Vdly__axi_sa16__DOT__compute_cycles;
    vlSelf->axi_sa16__DOT__warmup_cycles = __Vdly__axi_sa16__DOT__warmup_cycles;
    if (__Vdlyvset__axi_sa16__DOT__inputB_bank1__v0) {
        vlSelf->axi_sa16__DOT__inputB_bank1[__Vdlyvdim0__axi_sa16__DOT__inputB_bank1__v0] 
            = __Vdlyvval__axi_sa16__DOT__inputB_bank1__v0;
    }
    if (__Vdlyvset__axi_sa16__DOT__prev_outputC__v0) {
        __Vilp = 0U;
        while ((__Vilp <= 0x3fU)) {
            vlSelf->axi_sa16__DOT__prev_outputC[__Vilp] = 0U;
            __Vilp = ((IData)(1U) + __Vilp);
        }
    }
    if (__Vdlyvset__axi_sa16__DOT__prev_outputC__v64) {
        vlSelf->axi_sa16__DOT__prev_outputC[0U] = 0U;
    }
    if (__Vdlyvset__axi_sa16__DOT__prev_outputC__v65) {
        vlSelf->axi_sa16__DOT__prev_outputC[1U] = 0U;
    }
    if (__Vdlyvset__axi_sa16__DOT__prev_outputC__v66) {
        __Vilp = 2U;
        while ((__Vilp <= 0x3fU)) {
            vlSelf->axi_sa16__DOT__prev_outputC[__Vilp] = 0U;
            __Vilp = ((IData)(1U) + __Vilp);
        }
    }
    if (__Vdlyvset__axi_sa16__DOT__prev_outputC__v128) {
        vlSelf->axi_sa16__DOT__prev_outputC[0U] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v128;
        vlSelf->axi_sa16__DOT__prev_outputC[1U] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v129;
        vlSelf->axi_sa16__DOT__prev_outputC[2U] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v130;
        vlSelf->axi_sa16__DOT__prev_outputC[3U] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v131;
        vlSelf->axi_sa16__DOT__prev_outputC[4U] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v132;
        vlSelf->axi_sa16__DOT__prev_outputC[5U] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v133;
        vlSelf->axi_sa16__DOT__prev_outputC[6U] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v134;
        vlSelf->axi_sa16__DOT__prev_outputC[7U] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v135;
        vlSelf->axi_sa16__DOT__prev_outputC[8U] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v136;
        vlSelf->axi_sa16__DOT__prev_outputC[9U] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v137;
        vlSelf->axi_sa16__DOT__prev_outputC[0xaU] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v138;
        vlSelf->axi_sa16__DOT__prev_outputC[0xbU] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v139;
        vlSelf->axi_sa16__DOT__prev_outputC[0xcU] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v140;
        vlSelf->axi_sa16__DOT__prev_outputC[0xdU] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v141;
        vlSelf->axi_sa16__DOT__prev_outputC[0xeU] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v142;
        vlSelf->axi_sa16__DOT__prev_outputC[0xfU] = __Vdlyvval__axi_sa16__DOT__prev_outputC__v143;
        vlSelf->axi_sa16__DOT__prev_outputC[0x10U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v144;
        vlSelf->axi_sa16__DOT__prev_outputC[0x11U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v145;
        vlSelf->axi_sa16__DOT__prev_outputC[0x12U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v146;
        vlSelf->axi_sa16__DOT__prev_outputC[0x13U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v147;
        vlSelf->axi_sa16__DOT__prev_outputC[0x14U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v148;
        vlSelf->axi_sa16__DOT__prev_outputC[0x15U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v149;
        vlSelf->axi_sa16__DOT__prev_outputC[0x16U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v150;
        vlSelf->axi_sa16__DOT__prev_outputC[0x17U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v151;
        vlSelf->axi_sa16__DOT__prev_outputC[0x18U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v152;
        vlSelf->axi_sa16__DOT__prev_outputC[0x19U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v153;
        vlSelf->axi_sa16__DOT__prev_outputC[0x1aU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v154;
        vlSelf->axi_sa16__DOT__prev_outputC[0x1bU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v155;
        vlSelf->axi_sa16__DOT__prev_outputC[0x1cU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v156;
        vlSelf->axi_sa16__DOT__prev_outputC[0x1dU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v157;
        vlSelf->axi_sa16__DOT__prev_outputC[0x1eU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v158;
        vlSelf->axi_sa16__DOT__prev_outputC[0x1fU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v159;
        vlSelf->axi_sa16__DOT__prev_outputC[0x20U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v160;
        vlSelf->axi_sa16__DOT__prev_outputC[0x21U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v161;
        vlSelf->axi_sa16__DOT__prev_outputC[0x22U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v162;
        vlSelf->axi_sa16__DOT__prev_outputC[0x23U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v163;
        vlSelf->axi_sa16__DOT__prev_outputC[0x24U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v164;
        vlSelf->axi_sa16__DOT__prev_outputC[0x25U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v165;
        vlSelf->axi_sa16__DOT__prev_outputC[0x26U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v166;
        vlSelf->axi_sa16__DOT__prev_outputC[0x27U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v167;
        vlSelf->axi_sa16__DOT__prev_outputC[0x28U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v168;
        vlSelf->axi_sa16__DOT__prev_outputC[0x29U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v169;
        vlSelf->axi_sa16__DOT__prev_outputC[0x2aU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v170;
        vlSelf->axi_sa16__DOT__prev_outputC[0x2bU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v171;
        vlSelf->axi_sa16__DOT__prev_outputC[0x2cU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v172;
        vlSelf->axi_sa16__DOT__prev_outputC[0x2dU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v173;
        vlSelf->axi_sa16__DOT__prev_outputC[0x2eU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v174;
        vlSelf->axi_sa16__DOT__prev_outputC[0x2fU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v175;
        vlSelf->axi_sa16__DOT__prev_outputC[0x30U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v176;
        vlSelf->axi_sa16__DOT__prev_outputC[0x31U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v177;
        vlSelf->axi_sa16__DOT__prev_outputC[0x32U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v178;
        vlSelf->axi_sa16__DOT__prev_outputC[0x33U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v179;
        vlSelf->axi_sa16__DOT__prev_outputC[0x34U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v180;
        vlSelf->axi_sa16__DOT__prev_outputC[0x35U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v181;
        vlSelf->axi_sa16__DOT__prev_outputC[0x36U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v182;
        vlSelf->axi_sa16__DOT__prev_outputC[0x37U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v183;
        vlSelf->axi_sa16__DOT__prev_outputC[0x38U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v184;
        vlSelf->axi_sa16__DOT__prev_outputC[0x39U] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v185;
        vlSelf->axi_sa16__DOT__prev_outputC[0x3aU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v186;
        vlSelf->axi_sa16__DOT__prev_outputC[0x3bU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v187;
        vlSelf->axi_sa16__DOT__prev_outputC[0x3cU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v188;
        vlSelf->axi_sa16__DOT__prev_outputC[0x3dU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v189;
        vlSelf->axi_sa16__DOT__prev_outputC[0x3eU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v190;
        vlSelf->axi_sa16__DOT__prev_outputC[0x3fU] 
            = __Vdlyvval__axi_sa16__DOT__prev_outputC__v191;
    }
    vlSelf->axi_sa16__DOT__bank_comp = __Vdly__axi_sa16__DOT__bank_comp;
    vlSelf->axi_sa16__DOT__sa_state = __Vdly__axi_sa16__DOT__sa_state;
    vlSelf->axi_sa16__DOT__tile_ready1 = __Vdly__axi_sa16__DOT__tile_ready1;
    vlSelf->axi_sa16__DOT__tile_ready0 = __Vdly__axi_sa16__DOT__tile_ready0;
    if (__Vdlyvset__axi_sa16__DOT__inputA_bank1__v0) {
        vlSelf->axi_sa16__DOT__inputA_bank1[__Vdlyvdim0__axi_sa16__DOT__inputA_bank1__v0] 
            = __Vdlyvval__axi_sa16__DOT__inputA_bank1__v0;
    }
    if (__Vdlyvset__axi_sa16__DOT__inputA_bank1__v1) {
        vlSelf->axi_sa16__DOT__inputA_bank1[__Vdlyvdim0__axi_sa16__DOT__inputA_bank1__v1] 
            = __Vdlyvval__axi_sa16__DOT__inputA_bank1__v1;
    }
    if (__Vdlyvset__axi_sa16__DOT__inputA_bank0__v0) {
        vlSelf->axi_sa16__DOT__inputA_bank0[__Vdlyvdim0__axi_sa16__DOT__inputA_bank0__v0] 
            = __Vdlyvval__axi_sa16__DOT__inputA_bank0__v0;
    }
    if (__Vdlyvset__axi_sa16__DOT__inputA_bank0__v1) {
        vlSelf->axi_sa16__DOT__inputA_bank0[__Vdlyvdim0__axi_sa16__DOT__inputA_bank0__v1] 
            = __Vdlyvval__axi_sa16__DOT__inputA_bank0__v1;
    }
    if (__Vdlyvset__axi_sa16__DOT__inputB_bank0__v0) {
        vlSelf->axi_sa16__DOT__inputB_bank0[__Vdlyvdim0__axi_sa16__DOT__inputB_bank0__v0] 
            = __Vdlyvval__axi_sa16__DOT__inputB_bank0__v0;
    }
    if (__Vdlyvset__axi_sa16__DOT__inputB_bank0__v1) {
        vlSelf->axi_sa16__DOT__inputB_bank1[__Vdlyvdim0__axi_sa16__DOT__inputB_bank1__v1] 
            = __Vdlyvval__axi_sa16__DOT__inputB_bank1__v1;
        vlSelf->axi_sa16__DOT__inputB_bank0[__Vdlyvdim0__axi_sa16__DOT__inputB_bank0__v1] 
            = __Vdlyvval__axi_sa16__DOT__inputB_bank0__v1;
    }
    vlSelf->axi_sa16__DOT__final_output_detected = __Vdly__axi_sa16__DOT__final_output_detected;
    if (__Vdlyvset__axi_sa16__DOT__A_buf__v0) {
        vlSelf->axi_sa16__DOT__A_buf[0U] = __Vdlyvval__axi_sa16__DOT__A_buf__v0;
    }
    if (__Vdlyvset__axi_sa16__DOT__A_buf__v1) {
        vlSelf->axi_sa16__DOT__A_buf[1U] = __Vdlyvval__axi_sa16__DOT__A_buf__v1;
        vlSelf->axi_sa16__DOT__A_buf[2U] = __Vdlyvval__axi_sa16__DOT__A_buf__v2;
        vlSelf->axi_sa16__DOT__A_buf[3U] = __Vdlyvval__axi_sa16__DOT__A_buf__v3;
        vlSelf->axi_sa16__DOT__A_buf[4U] = __Vdlyvval__axi_sa16__DOT__A_buf__v4;
        vlSelf->axi_sa16__DOT__A_buf[5U] = __Vdlyvval__axi_sa16__DOT__A_buf__v5;
        vlSelf->axi_sa16__DOT__A_buf[6U] = __Vdlyvval__axi_sa16__DOT__A_buf__v6;
        vlSelf->axi_sa16__DOT__A_buf[7U] = __Vdlyvval__axi_sa16__DOT__A_buf__v7;
        vlSelf->axi_sa16__DOT__A_buf[8U] = __Vdlyvval__axi_sa16__DOT__A_buf__v8;
        vlSelf->axi_sa16__DOT__A_buf[9U] = __Vdlyvval__axi_sa16__DOT__A_buf__v9;
        vlSelf->axi_sa16__DOT__A_buf[0xaU] = __Vdlyvval__axi_sa16__DOT__A_buf__v10;
        vlSelf->axi_sa16__DOT__A_buf[0xbU] = __Vdlyvval__axi_sa16__DOT__A_buf__v11;
        vlSelf->axi_sa16__DOT__A_buf[0xcU] = __Vdlyvval__axi_sa16__DOT__A_buf__v12;
        vlSelf->axi_sa16__DOT__A_buf[0xdU] = __Vdlyvval__axi_sa16__DOT__A_buf__v13;
        vlSelf->axi_sa16__DOT__A_buf[0xeU] = __Vdlyvval__axi_sa16__DOT__A_buf__v14;
        vlSelf->axi_sa16__DOT__A_buf[0xfU] = __Vdlyvval__axi_sa16__DOT__A_buf__v15;
    }
    if (__Vdlyvset__axi_sa16__DOT__A_buf__v16) {
        vlSelf->axi_sa16__DOT__A_buf[0U] = __Vdlyvval__axi_sa16__DOT__A_buf__v16;
    }
    if (__Vdlyvset__axi_sa16__DOT__A_buf__v17) {
        vlSelf->axi_sa16__DOT__A_buf[1U] = __Vdlyvval__axi_sa16__DOT__A_buf__v17;
        vlSelf->axi_sa16__DOT__A_buf[2U] = __Vdlyvval__axi_sa16__DOT__A_buf__v18;
        vlSelf->axi_sa16__DOT__A_buf[3U] = __Vdlyvval__axi_sa16__DOT__A_buf__v19;
        vlSelf->axi_sa16__DOT__A_buf[4U] = __Vdlyvval__axi_sa16__DOT__A_buf__v20;
        vlSelf->axi_sa16__DOT__A_buf[5U] = __Vdlyvval__axi_sa16__DOT__A_buf__v21;
        vlSelf->axi_sa16__DOT__A_buf[6U] = __Vdlyvval__axi_sa16__DOT__A_buf__v22;
        vlSelf->axi_sa16__DOT__A_buf[7U] = __Vdlyvval__axi_sa16__DOT__A_buf__v23;
        vlSelf->axi_sa16__DOT__A_buf[8U] = __Vdlyvval__axi_sa16__DOT__A_buf__v24;
        vlSelf->axi_sa16__DOT__A_buf[9U] = __Vdlyvval__axi_sa16__DOT__A_buf__v25;
        vlSelf->axi_sa16__DOT__A_buf[0xaU] = __Vdlyvval__axi_sa16__DOT__A_buf__v26;
        vlSelf->axi_sa16__DOT__A_buf[0xbU] = __Vdlyvval__axi_sa16__DOT__A_buf__v27;
        vlSelf->axi_sa16__DOT__A_buf[0xcU] = __Vdlyvval__axi_sa16__DOT__A_buf__v28;
        vlSelf->axi_sa16__DOT__A_buf[0xdU] = __Vdlyvval__axi_sa16__DOT__A_buf__v29;
        vlSelf->axi_sa16__DOT__A_buf[0xeU] = __Vdlyvval__axi_sa16__DOT__A_buf__v30;
        vlSelf->axi_sa16__DOT__A_buf[0xfU] = __Vdlyvval__axi_sa16__DOT__A_buf__v31;
    }
    if (__Vdlyvset__axi_sa16__DOT__B_buf__v0) {
        vlSelf->axi_sa16__DOT__B_buf[0U] = __Vdlyvval__axi_sa16__DOT__B_buf__v0;
    }
    if (__Vdlyvset__axi_sa16__DOT__B_buf__v1) {
        vlSelf->axi_sa16__DOT__B_buf[1U] = __Vdlyvval__axi_sa16__DOT__B_buf__v1;
        vlSelf->axi_sa16__DOT__B_buf[2U] = __Vdlyvval__axi_sa16__DOT__B_buf__v2;
        vlSelf->axi_sa16__DOT__B_buf[3U] = __Vdlyvval__axi_sa16__DOT__B_buf__v3;
        vlSelf->axi_sa16__DOT__B_buf[4U] = __Vdlyvval__axi_sa16__DOT__B_buf__v4;
        vlSelf->axi_sa16__DOT__B_buf[5U] = __Vdlyvval__axi_sa16__DOT__B_buf__v5;
        vlSelf->axi_sa16__DOT__B_buf[6U] = __Vdlyvval__axi_sa16__DOT__B_buf__v6;
        vlSelf->axi_sa16__DOT__B_buf[7U] = __Vdlyvval__axi_sa16__DOT__B_buf__v7;
        vlSelf->axi_sa16__DOT__B_buf[8U] = __Vdlyvval__axi_sa16__DOT__B_buf__v8;
        vlSelf->axi_sa16__DOT__B_buf[9U] = __Vdlyvval__axi_sa16__DOT__B_buf__v9;
        vlSelf->axi_sa16__DOT__B_buf[0xaU] = __Vdlyvval__axi_sa16__DOT__B_buf__v10;
        vlSelf->axi_sa16__DOT__B_buf[0xbU] = __Vdlyvval__axi_sa16__DOT__B_buf__v11;
        vlSelf->axi_sa16__DOT__B_buf[0xcU] = __Vdlyvval__axi_sa16__DOT__B_buf__v12;
        vlSelf->axi_sa16__DOT__B_buf[0xdU] = __Vdlyvval__axi_sa16__DOT__B_buf__v13;
        vlSelf->axi_sa16__DOT__B_buf[0xeU] = __Vdlyvval__axi_sa16__DOT__B_buf__v14;
        vlSelf->axi_sa16__DOT__B_buf[0xfU] = __Vdlyvval__axi_sa16__DOT__B_buf__v15;
        vlSelf->axi_sa16__DOT__B_buf[0x10U] = __Vdlyvval__axi_sa16__DOT__B_buf__v16;
        vlSelf->axi_sa16__DOT__B_buf[0x11U] = __Vdlyvval__axi_sa16__DOT__B_buf__v17;
        vlSelf->axi_sa16__DOT__B_buf[0x12U] = __Vdlyvval__axi_sa16__DOT__B_buf__v18;
        vlSelf->axi_sa16__DOT__B_buf[0x13U] = __Vdlyvval__axi_sa16__DOT__B_buf__v19;
        vlSelf->axi_sa16__DOT__B_buf[0x14U] = __Vdlyvval__axi_sa16__DOT__B_buf__v20;
        vlSelf->axi_sa16__DOT__B_buf[0x15U] = __Vdlyvval__axi_sa16__DOT__B_buf__v21;
        vlSelf->axi_sa16__DOT__B_buf[0x16U] = __Vdlyvval__axi_sa16__DOT__B_buf__v22;
        vlSelf->axi_sa16__DOT__B_buf[0x17U] = __Vdlyvval__axi_sa16__DOT__B_buf__v23;
        vlSelf->axi_sa16__DOT__B_buf[0x18U] = __Vdlyvval__axi_sa16__DOT__B_buf__v24;
        vlSelf->axi_sa16__DOT__B_buf[0x19U] = __Vdlyvval__axi_sa16__DOT__B_buf__v25;
        vlSelf->axi_sa16__DOT__B_buf[0x1aU] = __Vdlyvval__axi_sa16__DOT__B_buf__v26;
        vlSelf->axi_sa16__DOT__B_buf[0x1bU] = __Vdlyvval__axi_sa16__DOT__B_buf__v27;
        vlSelf->axi_sa16__DOT__B_buf[0x1cU] = __Vdlyvval__axi_sa16__DOT__B_buf__v28;
        vlSelf->axi_sa16__DOT__B_buf[0x1dU] = __Vdlyvval__axi_sa16__DOT__B_buf__v29;
        vlSelf->axi_sa16__DOT__B_buf[0x1eU] = __Vdlyvval__axi_sa16__DOT__B_buf__v30;
        vlSelf->axi_sa16__DOT__B_buf[0x1fU] = __Vdlyvval__axi_sa16__DOT__B_buf__v31;
        vlSelf->axi_sa16__DOT__B_buf[0x20U] = __Vdlyvval__axi_sa16__DOT__B_buf__v32;
        vlSelf->axi_sa16__DOT__B_buf[0x21U] = __Vdlyvval__axi_sa16__DOT__B_buf__v33;
        vlSelf->axi_sa16__DOT__B_buf[0x22U] = __Vdlyvval__axi_sa16__DOT__B_buf__v34;
        vlSelf->axi_sa16__DOT__B_buf[0x23U] = __Vdlyvval__axi_sa16__DOT__B_buf__v35;
        vlSelf->axi_sa16__DOT__B_buf[0x24U] = __Vdlyvval__axi_sa16__DOT__B_buf__v36;
        vlSelf->axi_sa16__DOT__B_buf[0x25U] = __Vdlyvval__axi_sa16__DOT__B_buf__v37;
        vlSelf->axi_sa16__DOT__B_buf[0x26U] = __Vdlyvval__axi_sa16__DOT__B_buf__v38;
        vlSelf->axi_sa16__DOT__B_buf[0x27U] = __Vdlyvval__axi_sa16__DOT__B_buf__v39;
        vlSelf->axi_sa16__DOT__B_buf[0x28U] = __Vdlyvval__axi_sa16__DOT__B_buf__v40;
        vlSelf->axi_sa16__DOT__B_buf[0x29U] = __Vdlyvval__axi_sa16__DOT__B_buf__v41;
        vlSelf->axi_sa16__DOT__B_buf[0x2aU] = __Vdlyvval__axi_sa16__DOT__B_buf__v42;
        vlSelf->axi_sa16__DOT__B_buf[0x2bU] = __Vdlyvval__axi_sa16__DOT__B_buf__v43;
        vlSelf->axi_sa16__DOT__B_buf[0x2cU] = __Vdlyvval__axi_sa16__DOT__B_buf__v44;
        vlSelf->axi_sa16__DOT__B_buf[0x2dU] = __Vdlyvval__axi_sa16__DOT__B_buf__v45;
        vlSelf->axi_sa16__DOT__B_buf[0x2eU] = __Vdlyvval__axi_sa16__DOT__B_buf__v46;
        vlSelf->axi_sa16__DOT__B_buf[0x2fU] = __Vdlyvval__axi_sa16__DOT__B_buf__v47;
        vlSelf->axi_sa16__DOT__B_buf[0x30U] = __Vdlyvval__axi_sa16__DOT__B_buf__v48;
        vlSelf->axi_sa16__DOT__B_buf[0x31U] = __Vdlyvval__axi_sa16__DOT__B_buf__v49;
        vlSelf->axi_sa16__DOT__B_buf[0x32U] = __Vdlyvval__axi_sa16__DOT__B_buf__v50;
        vlSelf->axi_sa16__DOT__B_buf[0x33U] = __Vdlyvval__axi_sa16__DOT__B_buf__v51;
        vlSelf->axi_sa16__DOT__B_buf[0x34U] = __Vdlyvval__axi_sa16__DOT__B_buf__v52;
        vlSelf->axi_sa16__DOT__B_buf[0x35U] = __Vdlyvval__axi_sa16__DOT__B_buf__v53;
        vlSelf->axi_sa16__DOT__B_buf[0x36U] = __Vdlyvval__axi_sa16__DOT__B_buf__v54;
        vlSelf->axi_sa16__DOT__B_buf[0x37U] = __Vdlyvval__axi_sa16__DOT__B_buf__v55;
        vlSelf->axi_sa16__DOT__B_buf[0x38U] = __Vdlyvval__axi_sa16__DOT__B_buf__v56;
        vlSelf->axi_sa16__DOT__B_buf[0x39U] = __Vdlyvval__axi_sa16__DOT__B_buf__v57;
        vlSelf->axi_sa16__DOT__B_buf[0x3aU] = __Vdlyvval__axi_sa16__DOT__B_buf__v58;
        vlSelf->axi_sa16__DOT__B_buf[0x3bU] = __Vdlyvval__axi_sa16__DOT__B_buf__v59;
        vlSelf->axi_sa16__DOT__B_buf[0x3cU] = __Vdlyvval__axi_sa16__DOT__B_buf__v60;
        vlSelf->axi_sa16__DOT__B_buf[0x3dU] = __Vdlyvval__axi_sa16__DOT__B_buf__v61;
        vlSelf->axi_sa16__DOT__B_buf[0x3eU] = __Vdlyvval__axi_sa16__DOT__B_buf__v62;
        vlSelf->axi_sa16__DOT__B_buf[0x3fU] = __Vdlyvval__axi_sa16__DOT__B_buf__v63;
    }
    vlSelf->axi_sa16__DOT__final_rise = ((IData)(vlSelf->axi_sa16__DOT__final_output_detected) 
                                         & (~ (IData)(vlSelf->axi_sa16__DOT__final_q)));
    vlSelf->axi_sa16__DOT__inputA[0U] = vlSelf->axi_sa16__DOT__A_buf
        [0U];
    vlSelf->axi_sa16__DOT__inputA[1U] = vlSelf->axi_sa16__DOT__A_buf
        [1U];
    vlSelf->axi_sa16__DOT__inputA[2U] = vlSelf->axi_sa16__DOT__A_buf
        [2U];
    vlSelf->axi_sa16__DOT__inputA[3U] = vlSelf->axi_sa16__DOT__A_buf
        [3U];
    vlSelf->axi_sa16__DOT__inputA[4U] = vlSelf->axi_sa16__DOT__A_buf
        [4U];
    vlSelf->axi_sa16__DOT__inputA[5U] = vlSelf->axi_sa16__DOT__A_buf
        [5U];
    vlSelf->axi_sa16__DOT__inputA[6U] = vlSelf->axi_sa16__DOT__A_buf
        [6U];
    vlSelf->axi_sa16__DOT__inputA[7U] = vlSelf->axi_sa16__DOT__A_buf
        [7U];
    vlSelf->axi_sa16__DOT__inputA[8U] = vlSelf->axi_sa16__DOT__A_buf
        [8U];
    vlSelf->axi_sa16__DOT__inputA[9U] = vlSelf->axi_sa16__DOT__A_buf
        [9U];
    vlSelf->axi_sa16__DOT__inputA[0xaU] = vlSelf->axi_sa16__DOT__A_buf
        [0xaU];
    vlSelf->axi_sa16__DOT__inputA[0xbU] = vlSelf->axi_sa16__DOT__A_buf
        [0xbU];
    vlSelf->axi_sa16__DOT__inputA[0xcU] = vlSelf->axi_sa16__DOT__A_buf
        [0xcU];
    vlSelf->axi_sa16__DOT__inputA[0xdU] = vlSelf->axi_sa16__DOT__A_buf
        [0xdU];
    vlSelf->axi_sa16__DOT__inputA[0xeU] = vlSelf->axi_sa16__DOT__A_buf
        [0xeU];
    vlSelf->axi_sa16__DOT__inputA[0xfU] = vlSelf->axi_sa16__DOT__A_buf
        [0xfU];
    __Vilp = 0U;
    while ((__Vilp <= 0x3fU)) {
        vlSelf->axi_sa16__DOT__inputB[__Vilp] = vlSelf->axi_sa16__DOT__B_buf
            [__Vilp];
        __Vilp = ((IData)(1U) + __Vilp);
    }
}

VL_INLINE_OPT void Vaxi_sa16___024root___combo__TOP__0(Vaxi_sa16___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vaxi_sa16__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaxi_sa16___024root___combo__TOP__0\n"); );
    // Body
    vlSelf->axi_sa16__DOT__s_axi_rvalid_next = ((IData)(vlSelf->axi_sa16__DOT__s_axi_rvalid_reg) 
                                                & (~ (IData)(vlSelf->s_axi_rready)));
    vlSelf->axi_sa16__DOT__mem_rd_en = 0U;
    vlSelf->axi_sa16__DOT__s_axi_rid_next = vlSelf->axi_sa16__DOT__s_axi_rid_reg;
    vlSelf->axi_sa16__DOT__s_axi_rlast_next = vlSelf->axi_sa16__DOT__s_axi_rlast_reg;
    vlSelf->axi_sa16__DOT__mem_wr_en = 0U;
    vlSelf->axi_sa16__DOT__s_axi_bid_next = vlSelf->axi_sa16__DOT__s_axi_bid_reg;
    vlSelf->axi_sa16__DOT__s_axi_bvalid_next = vlSelf->axi_sa16__DOT__s_axi_bvalid_reg;
    if ((0U != (IData)(vlSelf->axi_sa16__DOT__write_state))) {
        if ((1U == (IData)(vlSelf->axi_sa16__DOT__write_state))) {
            if (((IData)(vlSelf->s_axi_wvalid) & (IData)(vlSelf->s_axi_wready))) {
                vlSelf->axi_sa16__DOT__mem_wr_en = 1U;
                if (vlSelf->s_axi_wlast) {
                    vlSelf->axi_sa16__DOT__s_axi_bid_next 
                        = vlSelf->axi_sa16__DOT__write_id_reg;
                    vlSelf->axi_sa16__DOT__s_axi_bvalid_next = 1U;
                }
            }
        } else if ((2U == (IData)(vlSelf->axi_sa16__DOT__write_state))) {
            if (((IData)(vlSelf->s_axi_bvalid) & (IData)(vlSelf->s_axi_bready))) {
                vlSelf->axi_sa16__DOT__s_axi_bvalid_next = 0U;
            }
        }
    }
    vlSelf->axi_sa16__DOT__read_burst_next = vlSelf->axi_sa16__DOT__read_burst_reg;
    vlSelf->axi_sa16__DOT__read_id_next = vlSelf->axi_sa16__DOT__read_id_reg;
    vlSelf->axi_sa16__DOT__read_size_next = vlSelf->axi_sa16__DOT__read_size_reg;
    if ((1U & (~ (IData)(vlSelf->axi_sa16__DOT__read_state)))) {
        if (((IData)(vlSelf->s_axi_arvalid) & (IData)(vlSelf->s_axi_arready))) {
            vlSelf->axi_sa16__DOT__read_burst_next 
                = vlSelf->s_axi_arburst;
            vlSelf->axi_sa16__DOT__read_id_next = vlSelf->s_axi_arid;
            vlSelf->axi_sa16__DOT__read_size_next = 
                ((2U > (IData)(vlSelf->s_axi_arsize))
                  ? (7U & (IData)(vlSelf->s_axi_arsize))
                  : 2U);
        }
    }
    if (vlSelf->axi_sa16__DOT__read_state) {
        vlSelf->axi_sa16__DOT__read_state_next = 1U;
        vlSelf->axi_sa16__DOT__s_axi_arready_next = vlSelf->axi_sa16__DOT__s_axi_arready_reg;
        vlSelf->axi_sa16__DOT__read_count_next = vlSelf->axi_sa16__DOT__read_count_reg;
        vlSelf->axi_sa16__DOT__write_burst_next = vlSelf->axi_sa16__DOT__write_burst_reg;
        vlSelf->axi_sa16__DOT__write_size_next = vlSelf->axi_sa16__DOT__write_size_reg;
        vlSelf->axi_sa16__DOT__write_id_next = vlSelf->axi_sa16__DOT__write_id_reg;
        vlSelf->axi_sa16__DOT__s_axi_awready_next = vlSelf->axi_sa16__DOT__s_axi_awready_reg;
        vlSelf->axi_sa16__DOT__s_axi_wready_next = vlSelf->axi_sa16__DOT__s_axi_wready_reg;
        vlSelf->axi_sa16__DOT__write_state_next = vlSelf->axi_sa16__DOT__write_state;
        vlSelf->axi_sa16__DOT__read_addr_next = vlSelf->axi_sa16__DOT__read_addr_reg;
        if (vlSelf->axi_sa16__DOT__read_state) {
            if ((1U & ((IData)(vlSelf->s_axi_rready) 
                       | (~ (IData)(vlSelf->axi_sa16__DOT__s_axi_rvalid_reg))))) {
                vlSelf->axi_sa16__DOT__s_axi_rvalid_next = 1U;
                vlSelf->axi_sa16__DOT__mem_rd_en = 1U;
                vlSelf->axi_sa16__DOT__s_axi_rid_next 
                    = vlSelf->axi_sa16__DOT__read_id_reg;
                if ((0U == (IData)(vlSelf->axi_sa16__DOT__read_count_reg))) {
                    vlSelf->axi_sa16__DOT__s_axi_rlast_next = 1U;
                    vlSelf->axi_sa16__DOT__read_state_next = 0U;
                    vlSelf->axi_sa16__DOT__s_axi_arready_next = 1U;
                } else {
                    vlSelf->axi_sa16__DOT__s_axi_rlast_next = 0U;
                }
                if ((0U != (IData)(vlSelf->axi_sa16__DOT__read_count_reg))) {
                    vlSelf->axi_sa16__DOT__read_count_next 
                        = (0xffU & ((IData)(vlSelf->axi_sa16__DOT__read_count_reg) 
                                    - (IData)(1U)));
                }
                if ((0U != (IData)(vlSelf->axi_sa16__DOT__read_burst_reg))) {
                    vlSelf->axi_sa16__DOT__read_addr_next 
                        = (0xfffffU & (vlSelf->axi_sa16__DOT__read_addr_reg 
                                       + ((IData)(1U) 
                                          << (IData)(vlSelf->axi_sa16__DOT__read_size_reg))));
                }
            }
        }
    } else {
        vlSelf->axi_sa16__DOT__read_state_next = 0U;
        vlSelf->axi_sa16__DOT__s_axi_arready_next = vlSelf->axi_sa16__DOT__s_axi_arready_reg;
        vlSelf->axi_sa16__DOT__read_count_next = vlSelf->axi_sa16__DOT__read_count_reg;
        vlSelf->axi_sa16__DOT__write_burst_next = vlSelf->axi_sa16__DOT__write_burst_reg;
        vlSelf->axi_sa16__DOT__write_size_next = vlSelf->axi_sa16__DOT__write_size_reg;
        vlSelf->axi_sa16__DOT__write_id_next = vlSelf->axi_sa16__DOT__write_id_reg;
        vlSelf->axi_sa16__DOT__s_axi_awready_next = vlSelf->axi_sa16__DOT__s_axi_awready_reg;
        vlSelf->axi_sa16__DOT__s_axi_wready_next = vlSelf->axi_sa16__DOT__s_axi_wready_reg;
        vlSelf->axi_sa16__DOT__write_state_next = vlSelf->axi_sa16__DOT__write_state;
        vlSelf->axi_sa16__DOT__read_addr_next = vlSelf->axi_sa16__DOT__read_addr_reg;
        vlSelf->axi_sa16__DOT__s_axi_arready_next = 1U;
        if (((IData)(vlSelf->s_axi_arvalid) & (IData)(vlSelf->s_axi_arready))) {
            vlSelf->axi_sa16__DOT__read_state_next = 1U;
            vlSelf->axi_sa16__DOT__s_axi_arready_next = 0U;
            vlSelf->axi_sa16__DOT__read_count_next 
                = vlSelf->s_axi_arlen;
            vlSelf->axi_sa16__DOT__read_addr_next = vlSelf->s_axi_araddr;
        }
    }
    vlSelf->axi_sa16__DOT__write_addr_next = vlSelf->axi_sa16__DOT__write_addr_reg;
    if ((0U == (IData)(vlSelf->axi_sa16__DOT__write_state))) {
        vlSelf->axi_sa16__DOT__s_axi_awready_next = 1U;
        vlSelf->axi_sa16__DOT__s_axi_wready_next = 1U;
        if (((IData)(vlSelf->s_axi_awvalid) & (IData)(vlSelf->s_axi_awready))) {
            vlSelf->axi_sa16__DOT__write_burst_next 
                = vlSelf->s_axi_awburst;
            vlSelf->axi_sa16__DOT__write_size_next 
                = ((2U > (IData)(vlSelf->s_axi_awsize))
                    ? (7U & (IData)(vlSelf->s_axi_awsize))
                    : 2U);
            vlSelf->axi_sa16__DOT__write_id_next = vlSelf->s_axi_awid;
            vlSelf->axi_sa16__DOT__s_axi_awready_next = 0U;
            vlSelf->axi_sa16__DOT__s_axi_wready_next = 1U;
            vlSelf->axi_sa16__DOT__write_state_next = 1U;
            vlSelf->axi_sa16__DOT__write_addr_next 
                = vlSelf->s_axi_awaddr;
        }
    } else {
        if ((1U != (IData)(vlSelf->axi_sa16__DOT__write_state))) {
            if ((2U == (IData)(vlSelf->axi_sa16__DOT__write_state))) {
                if (((IData)(vlSelf->s_axi_bvalid) 
                     & (IData)(vlSelf->s_axi_bready))) {
                    vlSelf->axi_sa16__DOT__s_axi_awready_next = 1U;
                }
            }
        }
        if ((1U == (IData)(vlSelf->axi_sa16__DOT__write_state))) {
            vlSelf->axi_sa16__DOT__s_axi_wready_next = 1U;
            if (((IData)(vlSelf->s_axi_wvalid) & (IData)(vlSelf->s_axi_wready))) {
                if (vlSelf->s_axi_wlast) {
                    vlSelf->axi_sa16__DOT__s_axi_wready_next = 1U;
                    vlSelf->axi_sa16__DOT__write_state_next = 2U;
                }
                if ((0U != (IData)(vlSelf->axi_sa16__DOT__write_burst_reg))) {
                    vlSelf->axi_sa16__DOT__write_addr_next 
                        = (0xfffffU & (vlSelf->axi_sa16__DOT__write_addr_reg 
                                       + ((IData)(1U) 
                                          << (IData)(vlSelf->axi_sa16__DOT__write_size_reg))));
                }
            }
        } else if ((2U == (IData)(vlSelf->axi_sa16__DOT__write_state))) {
            if (((IData)(vlSelf->s_axi_bvalid) & (IData)(vlSelf->s_axi_bready))) {
                vlSelf->axi_sa16__DOT__write_state_next = 0U;
            }
        }
    }
    vlSelf->axi_sa16__DOT__w_hs = ((IData)(vlSelf->s_axi_wvalid) 
                                   & (IData)(vlSelf->s_axi_wready));
}

VL_INLINE_OPT void Vaxi_sa16___024root___sequent__TOP__8(Vaxi_sa16___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vaxi_sa16__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaxi_sa16___024root___sequent__TOP__8\n"); );
    // Body
    vlSelf->axi_sa16__DOT__outputC[0x3fU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_255__DOT__io_outputC_3_REG;
    vlSelf->axi_sa16__DOT__outputC[0x3eU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_255__DOT__io_outputC_2_REG;
    vlSelf->axi_sa16__DOT__outputC[0x3dU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_255__DOT__io_outputC_1_REG;
    vlSelf->axi_sa16__DOT__outputC[0x3cU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_255__DOT__io_outputC_0_REG;
    vlSelf->axi_sa16__DOT__outputC[0x3bU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_254__DOT__io_outputC_3_REG;
    vlSelf->axi_sa16__DOT__outputC[0x3aU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_254__DOT__io_outputC_2_REG;
    vlSelf->axi_sa16__DOT__outputC[0x39U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_254__DOT__io_outputC_1_REG;
    vlSelf->axi_sa16__DOT__outputC[0x38U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_254__DOT__io_outputC_0_REG;
    vlSelf->axi_sa16__DOT__outputC[0x37U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_253__DOT__io_outputC_3_REG;
    vlSelf->axi_sa16__DOT__outputC[0x36U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_253__DOT__io_outputC_2_REG;
    vlSelf->axi_sa16__DOT__outputC[0x35U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_253__DOT__io_outputC_1_REG;
    vlSelf->axi_sa16__DOT__outputC[0x34U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_253__DOT__io_outputC_0_REG;
    vlSelf->axi_sa16__DOT__outputC[0x33U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_252__DOT__io_outputC_3_REG;
    vlSelf->axi_sa16__DOT__outputC[0x32U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_252__DOT__io_outputC_2_REG;
    vlSelf->axi_sa16__DOT__outputC[0x31U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_252__DOT__io_outputC_1_REG;
    vlSelf->axi_sa16__DOT__outputC[0x30U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_252__DOT__io_outputC_0_REG;
    vlSelf->axi_sa16__DOT__outputC[0x2fU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_251__DOT__io_outputC_3_REG;
    vlSelf->axi_sa16__DOT__outputC[0x2eU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_251__DOT__io_outputC_2_REG;
    vlSelf->axi_sa16__DOT__outputC[0x2dU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_251__DOT__io_outputC_1_REG;
    vlSelf->axi_sa16__DOT__outputC[0x2cU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_251__DOT__io_outputC_0_REG;
    vlSelf->axi_sa16__DOT__outputC[0x2bU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_250__DOT__io_outputC_3_REG;
    vlSelf->axi_sa16__DOT__outputC[0x2aU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_250__DOT__io_outputC_2_REG;
    vlSelf->axi_sa16__DOT__outputC[0x29U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_250__DOT__io_outputC_1_REG;
    vlSelf->axi_sa16__DOT__outputC[0x28U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_250__DOT__io_outputC_0_REG;
    vlSelf->axi_sa16__DOT__outputC[0x27U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_249__DOT__io_outputC_3_REG;
    vlSelf->axi_sa16__DOT__outputC[0x26U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_249__DOT__io_outputC_2_REG;
    vlSelf->axi_sa16__DOT__outputC[0x25U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_249__DOT__io_outputC_1_REG;
    vlSelf->axi_sa16__DOT__outputC[0x24U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_249__DOT__io_outputC_0_REG;
    vlSelf->axi_sa16__DOT__outputC[0x23U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_248__DOT__io_outputC_3_REG;
    vlSelf->axi_sa16__DOT__outputC[0x22U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_248__DOT__io_outputC_2_REG;
    vlSelf->axi_sa16__DOT__outputC[0x21U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_248__DOT__io_outputC_1_REG;
    vlSelf->axi_sa16__DOT__outputC[0x20U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_248__DOT__io_outputC_0_REG;
    vlSelf->axi_sa16__DOT__outputC[0x1fU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_247__DOT__io_outputC_3_REG;
    vlSelf->axi_sa16__DOT__outputC[0x1eU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_247__DOT__io_outputC_2_REG;
    vlSelf->axi_sa16__DOT__outputC[0x1dU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_247__DOT__io_outputC_1_REG;
    vlSelf->axi_sa16__DOT__outputC[0x1cU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_247__DOT__io_outputC_0_REG;
    vlSelf->axi_sa16__DOT__outputC[0x1bU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_246__DOT__io_outputC_3_REG;
    vlSelf->axi_sa16__DOT__outputC[0x1aU] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_246__DOT__io_outputC_2_REG;
    vlSelf->axi_sa16__DOT__outputC[0x19U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_246__DOT__io_outputC_1_REG;
    vlSelf->axi_sa16__DOT__outputC[0x18U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_246__DOT__io_outputC_0_REG;
    vlSelf->axi_sa16__DOT__outputC[0x17U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_245__DOT__io_outputC_3_REG;
    vlSelf->axi_sa16__DOT__outputC[0x16U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_245__DOT__io_outputC_2_REG;
    vlSelf->axi_sa16__DOT__outputC[0x15U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_245__DOT__io_outputC_1_REG;
    vlSelf->axi_sa16__DOT__outputC[0x14U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_245__DOT__io_outputC_0_REG;
    vlSelf->axi_sa16__DOT__outputC[0x13U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_244__DOT__io_outputC_3_REG;
    vlSelf->axi_sa16__DOT__outputC[0x12U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_244__DOT__io_outputC_2_REG;
    vlSelf->axi_sa16__DOT__outputC[0x11U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_244__DOT__io_outputC_1_REG;
    vlSelf->axi_sa16__DOT__outputC[0x10U] = vlSelf->axi_sa16__DOT__uut__DOT__systolicTensorArray__DOT__group_processing_element_244__DOT__io_outputC_0_REG;
    vlSelf->axi_sa16__DOT__outputC[0xfU] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_15_r;
    vlSelf->axi_sa16__DOT__outputC[0xeU] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_14_r_1;
    vlSelf->axi_sa16__DOT__outputC[0xdU] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_13_r_2;
    vlSelf->axi_sa16__DOT__outputC[0xcU] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_12_r_3;
    vlSelf->axi_sa16__DOT__outputC[0xbU] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_11_r_4;
    vlSelf->axi_sa16__DOT__outputC[0xaU] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_10_r_5;
    vlSelf->axi_sa16__DOT__outputC[9U] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_9_r_6;
    vlSelf->axi_sa16__DOT__outputC[8U] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_8_r_7;
    vlSelf->axi_sa16__DOT__outputC[7U] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_7_r_8;
    vlSelf->axi_sa16__DOT__outputC[6U] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_6_r_9;
    vlSelf->axi_sa16__DOT__outputC[5U] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_5_r_10;
    vlSelf->axi_sa16__DOT__outputC[4U] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_4_r_11;
    vlSelf->axi_sa16__DOT__outputC[3U] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_3_r_12;
    vlSelf->axi_sa16__DOT__outputC[2U] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_2_r_13;
    vlSelf->axi_sa16__DOT__outputC[1U] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_1_r_14;
    vlSelf->axi_sa16__DOT__outputC[0U] = vlSelf->axi_sa16__DOT__uut__DOT__postProcessor__DOT__io_output_0_r_15;
}

void Vaxi_sa16___024root___sequent__TOP__2(Vaxi_sa16___024root* vlSelf);
void Vaxi_sa16___024root___sequent__TOP__3(Vaxi_sa16___024root* vlSelf);
void Vaxi_sa16___024root___sequent__TOP__4(Vaxi_sa16___024root* vlSelf);
void Vaxi_sa16___024root___sequent__TOP__5(Vaxi_sa16___024root* vlSelf);
void Vaxi_sa16___024root___sequent__TOP__6(Vaxi_sa16___024root* vlSelf);

void Vaxi_sa16___024root___eval(Vaxi_sa16___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vaxi_sa16__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaxi_sa16___024root___eval\n"); );
    // Body
    if (((IData)(vlSelf->clk) & (~ (IData)(vlSelf->__Vclklast__TOP__clk)))) {
        Vaxi_sa16___024root___sequent__TOP__2(vlSelf);
        Vaxi_sa16___024root___sequent__TOP__3(vlSelf);
        Vaxi_sa16___024root___sequent__TOP__4(vlSelf);
        Vaxi_sa16___024root___sequent__TOP__5(vlSelf);
        Vaxi_sa16___024root___sequent__TOP__6(vlSelf);
    }
    if ((((IData)(vlSelf->clk) & (~ (IData)(vlSelf->__Vclklast__TOP__clk))) 
         | ((IData)(vlSelf->rst) & (~ (IData)(vlSelf->__Vclklast__TOP__rst))))) {
        Vaxi_sa16___024root___sequent__TOP__7(vlSelf);
    }
    Vaxi_sa16___024root___combo__TOP__0(vlSelf);
    if (((IData)(vlSelf->clk) & (~ (IData)(vlSelf->__Vclklast__TOP__clk)))) {
        Vaxi_sa16___024root___sequent__TOP__8(vlSelf);
    }
    // Final
    vlSelf->__Vclklast__TOP__clk = vlSelf->clk;
    vlSelf->__Vclklast__TOP__rst = vlSelf->rst;
}

#ifdef VL_DEBUG
void Vaxi_sa16___024root___eval_debug_assertions(Vaxi_sa16___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vaxi_sa16__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vaxi_sa16___024root___eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((vlSelf->clk & 0xfeU))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY((vlSelf->rst & 0xfeU))) {
        Verilated::overWidthError("rst");}
    if (VL_UNLIKELY((vlSelf->s_axi_awaddr & 0xfff00000U))) {
        Verilated::overWidthError("s_axi_awaddr");}
    if (VL_UNLIKELY((vlSelf->s_axi_awsize & 0xf8U))) {
        Verilated::overWidthError("s_axi_awsize");}
    if (VL_UNLIKELY((vlSelf->s_axi_awburst & 0xfcU))) {
        Verilated::overWidthError("s_axi_awburst");}
    if (VL_UNLIKELY((vlSelf->s_axi_awlock & 0xfeU))) {
        Verilated::overWidthError("s_axi_awlock");}
    if (VL_UNLIKELY((vlSelf->s_axi_awcache & 0xf0U))) {
        Verilated::overWidthError("s_axi_awcache");}
    if (VL_UNLIKELY((vlSelf->s_axi_awprot & 0xf8U))) {
        Verilated::overWidthError("s_axi_awprot");}
    if (VL_UNLIKELY((vlSelf->s_axi_awvalid & 0xfeU))) {
        Verilated::overWidthError("s_axi_awvalid");}
    if (VL_UNLIKELY((vlSelf->s_axi_wstrb & 0xf0U))) {
        Verilated::overWidthError("s_axi_wstrb");}
    if (VL_UNLIKELY((vlSelf->s_axi_wlast & 0xfeU))) {
        Verilated::overWidthError("s_axi_wlast");}
    if (VL_UNLIKELY((vlSelf->s_axi_wvalid & 0xfeU))) {
        Verilated::overWidthError("s_axi_wvalid");}
    if (VL_UNLIKELY((vlSelf->s_axi_bready & 0xfeU))) {
        Verilated::overWidthError("s_axi_bready");}
    if (VL_UNLIKELY((vlSelf->s_axi_araddr & 0xfff00000U))) {
        Verilated::overWidthError("s_axi_araddr");}
    if (VL_UNLIKELY((vlSelf->s_axi_arsize & 0xf8U))) {
        Verilated::overWidthError("s_axi_arsize");}
    if (VL_UNLIKELY((vlSelf->s_axi_arburst & 0xfcU))) {
        Verilated::overWidthError("s_axi_arburst");}
    if (VL_UNLIKELY((vlSelf->s_axi_arlock & 0xfeU))) {
        Verilated::overWidthError("s_axi_arlock");}
    if (VL_UNLIKELY((vlSelf->s_axi_arcache & 0xf0U))) {
        Verilated::overWidthError("s_axi_arcache");}
    if (VL_UNLIKELY((vlSelf->s_axi_arprot & 0xf8U))) {
        Verilated::overWidthError("s_axi_arprot");}
    if (VL_UNLIKELY((vlSelf->s_axi_arvalid & 0xfeU))) {
        Verilated::overWidthError("s_axi_arvalid");}
    if (VL_UNLIKELY((vlSelf->s_axi_rready & 0xfeU))) {
        Verilated::overWidthError("s_axi_rready");}
}
#endif  // VL_DEBUG
