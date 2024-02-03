
<!-- README.md is generated from README.Rmd. Please edit that file -->

# RNGT

<!-- badges: start -->

[![Lifecycle:
experimental](https://img.shields.io/badge/lifecycle-experimental-orange.svg)](https://lifecycle.r-lib.org/articles/stages.html#experimental)
<!-- badges: end -->

R wrappers for [NGT](https://github.com/yahoojapan/NGT), which provides
high-speed approximate nearest neighbor searches against a large volume
of data in high dimensional vector data space.

## Installation

You can install the development version of RNGT from
[GitHub](https://github.com/paithiov909/RNGT) with:

``` r
remotes::install_github("paithiov909/RNGT")
```

## Usage

``` r
require(RNGT)

vectors <- runif(100 * 10, max = 100) |>
  matrix(nrow = 100, ncol = 10)

query <- vectors[1, ]

dir <- tempdir()
index <- RNGT::NgtIndex$new(dir)
index$create(dimension = ncol(vectors))

# since `create` just creates an empty index,
# you must `open` the index to insert new data.
index$open()
index$batch_insert(vectors)
#>   [1]   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18
#>  [19]  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36
#>  [37]  37  38  39  40  41  42  43  44  45  46  47  48  49  50  51  52  53  54
#>  [55]  55  56  57  58  59  60  61  62  63  64  65  66  67  68  69  70  71  72
#>  [73]  73  74  75  76  77  78  79  80  81  82  83  84  85  86  87  88  89  90
#>  [91]  91  92  93  94  95  96  97  98  99 100
index$save()

results <- index$search(query, k = 5L)
results
#> # A tibble: 5 × 2
#>      id distance
#>   <int>    <dbl>
#> 1     1      0  
#> 2    89     63.3
#> 3    97     65.3
#> 4    92     70.0
#> 5    10     76.1

index$close()
```
