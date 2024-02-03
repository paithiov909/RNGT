#' @useDynLib RNGT, .registration = TRUE
#' @import Rcpp
#' @import methods
#' @importFrom rlang .env
#' @importFrom R6 R6Class
#' @importFrom tibble as_tibble
#' @keywords internal
"_PACKAGE"

utils::globalVariables(
  c(".mod", ".env", "self")
)

.mod <- Rcpp::Module("RNGT")

.onUnload <- function(libpath) {
  library.dynam.unload("RNGT", libpath)
}

#' Make a random name
#' @param prefix A single character string
#' @param len An integer for the number of random characters
#' @return A character string with the prefix and random letters separated by
#' and underscore.
#' @export
#' @keywords internal
# recipes::rand_id(): https://github.com/tidymodels/recipes/blob/main/R/misc.R
rand_name <- function(prefix = "index", len = 5) {
  candidates <- c(letters, LETTERS, paste(0:9))
  paste(prefix,
    paste0(sample(candidates, len, replace = TRUE), collapse = ""),
    sep = "_"
  )
}
