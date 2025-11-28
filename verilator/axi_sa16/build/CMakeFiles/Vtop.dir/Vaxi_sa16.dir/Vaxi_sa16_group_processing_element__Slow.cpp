// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vaxi_sa16.h for the primary calling header

#include "verilated.h"

#include "Vaxi_sa16__Syms.h"
#include "Vaxi_sa16_group_processing_element.h"

void Vaxi_sa16_group_processing_element___ctor_var_reset(Vaxi_sa16_group_processing_element* vlSelf);

Vaxi_sa16_group_processing_element::Vaxi_sa16_group_processing_element(Vaxi_sa16__Syms* symsp, const char* name)
    : VerilatedModule{name}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vaxi_sa16_group_processing_element___ctor_var_reset(this);
}

void Vaxi_sa16_group_processing_element::__Vconfigure(bool first) {
    if (false && first) {}  // Prevent unused
}

Vaxi_sa16_group_processing_element::~Vaxi_sa16_group_processing_element() {
}
