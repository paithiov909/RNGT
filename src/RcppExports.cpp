// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// rcpp_hello_world
int rcpp_hello_world(std::string anng_path, std::string input_data);
RcppExport SEXP _RNGT_rcpp_hello_world(SEXP anng_pathSEXP, SEXP input_dataSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type anng_path(anng_pathSEXP);
    Rcpp::traits::input_parameter< std::string >::type input_data(input_dataSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_hello_world(anng_path, input_data));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_RNGT_rcpp_hello_world", (DL_FUNC) &_RNGT_rcpp_hello_world, 2},
    {NULL, NULL, 0}
};

RcppExport void R_init_RNGT(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}