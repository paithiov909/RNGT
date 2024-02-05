
<!-- README.md is generated from README.Rmd. Please edit that file -->

# RNGT

<!-- badges: start -->

[![Lifecycle:
experimental](https://img.shields.io/badge/lifecycle-experimental-orange.svg)](https://lifecycle.r-lib.org/articles/stages.html#experimental)
[![R-CMD-check](https://github.com/paithiov909/RNGT/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/paithiov909/RNGT/actions/workflows/R-CMD-check.yaml)
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

data("gen7singles2018", package = "RNGT")

vectors <-
  dplyr::select(
    gen7singles2018,
    !starts_with("name")
  ) |>
  as.matrix()

query <-
  vectors[gen7singles2018$name_en == "blissey", ]

index <- RNGT::NgtIndex$
  new(tempdir())$
  create(
    dimension = ncol(vectors),
    distance_type = "cosine"
  )

# since `create` just creates an empty index,
# you must `open` the index to insert new data.
index$
  open()$
  batch_insert(vectors)
index$save()

results <- index$search(query, k = 10L)
gen7singles2018 |>
  dplyr::select(
    name_en, name_ja
  ) |>
  dplyr::mutate(id = dplyr::row_number()) |>
  dplyr::inner_join(results, by = "id") |>
  dplyr::arrange(distance)
#> # A tibble: 10 × 4
#>    name_en    name_ja       id distance
#>    <chr>      <chr>      <int>    <dbl>
#>  1 blissey    ハピナス      62    0    
#>  2 chansey    ラッキー      90    0.695
#>  3 espeon     エーフィ      83    0.731
#>  4 umbreon    ブラッキー    48    0.747
#>  5 cloyster   パルシェン   122    0.760
#>  6 pinsir     カイロス      34    0.774
#>  7 whimsicott エルフーン   143    0.780
#>  8 reuniclus  ランクルス    49    0.801
#>  9 banette    ジュペッタ    29    0.803
#> 10 braviary   ウォーグル    45    0.805

index$close()
```
