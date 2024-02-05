#' R6 Class for Graph and Tree Based Index
#'
#' @description
#' Graph and tree based index.
#'
#' @export
NgtIndex <- R6::R6Class("NgtIndex", # nolint
  public = list(
    #' @description
    #' Creates a new `NgtIndex` object.
    #' @param path  path to the NGT index.
    #' @param sub_dir sub-directory of the index.
    #' @return a new `NgtIndex` object.
    initialize = function(path, sub_dir = rand_name("NgtIndex")) {
      self$path <- file.path(path, sub_dir)
    },

    #' @description
    #' Opens a NGT index.
    #' @param read_only whether the index is read only.
    #' @param tree_disabled whether the tree is disabled.
    #' @param log_disabled whether the log is disabled.
    open = function(read_only = FALSE,
                    tree_disabled = FALSE,
                    log_disabled = FALSE) {
      if (!is.null(private$index)) {
        rlang::abort(
          "the index is already opened."
        )
      }
      private$index <- methods::new(
        .mod$Index,
        self$path,
        read_only,
        tree_disabled,
        log_disabled
      )
      invisible(self)
    },

    #' @description
    #' Creates an empty index with the specified parameters.
    #' @param dimension dimension of the vectors.
    #' @param edge_size_for_creation number of edges for each node in the graph.
    #' @param edge_size_for_search number of edges to search.
    #' @param distance_type distance type.
    #' @param object_type object type.
    #' @param sub_dir sub directory to store the index in.
    create = function(dimension,
                      edge_size_for_creation = 10,
                      edge_size_for_search = 40,
                      distance_type = c("l2", "l1", "normalized_l2", "hamming", "jaccard", "sparse_jaccard", "angle", "normalized_angle", "cosine", "normalized_cosine", "normalized_l2"), # nolint
                      object_type = c("float", "byte", "float16")) {
      distance_type <- rlang::arg_match(distance_type)
      object_type <- rlang::arg_match(object_type)

      distance_type <- switch(distance_type,
        l1 = "L1",
        l2 = "L2",
        normalized_l2 = "Normalized L2",
        hamming = "Hamming",
        jaccard = "Jaccard",
        sparse_jaccard = "Sparse Jaccard",
        angle = "Angle",
        normalized_angle = "Normalized Angle",
        cosine = "Cosine",
        normalized_cosine = "Normalized Cosine",
        normalized_l2 = "Normalized L2"
      )
      # once create a new index via `factory`,
      # then close the index.
      idx <- methods::new(
        .mod$Index,
        self$path,
        dimension,
        edge_size_for_creation,
        edge_size_for_search,
        distance_type,
        object_type
      )
      idx$close()
      invisible(self)
    },

    #' @description
    #' Searches for the k approximate nearest neighbors
    #' of the specifiecd query object.
    #' @param query query object.
    #' @param k number of nearest neighbors.
    #' @param epsilon epsilon which defines the explored range for the graph
    #' @param edge_size number of edges for each node to explore the graph
    #' @param expected_accuracy expected accuracy.
    #' @param with_distance whether to return distance.
    #' @return tibble.
    search = function(query,
                      k = 20L,
                      epsilon = 0.1,
                      edge_size = -1L,
                      expected_accuracy = -1.0,
                      with_distance = TRUE) {
      private$check_index_opened()

      query <- as.double(query)
      if (!is.integer(c(k, edge_size))) {
        rlang::abort("`k` and `edge_size` must be integer.")
      }
      if (!is.double(c(epsilon, expected_accuracy))) {
        rlang::abort("`epsilon` and `expected_accuracy` must be numeric.")
      }
      if (anyNA(query) || !all(is.finite(query))) {
        rlang::abort("`query` must be a finite numeric vector.")
      }
      tibble::as_tibble(
        private$index$search(
          query,
          k, epsilon, edge_size, expected_accuracy, with_distance
        ),
        .name_repair = "unique"
      )
    },

    #' @description
    #' Saves the index.
    #' @param path path to save the index. defaults to the path of the index.
    save = function(path) {
      private$check_index_opened()

      if (missing(path)) {
        path <- self$path
      }
      private$index$save(path)
      invisible(self)
    },

    #' @description
    #' Removes objects from the index by their IDs.
    #' @param ids IDs of the objects to be removed.
    #' @return integers; `ids` is returned invisibly as is.
    remove = function(ids) {
      private$check_index_opened()

      ids <- as.integer(ids)
      private$index$remove(ids)
      invisible(ids)
    },

    #' @description
    #' Refines the index with the specified parameters.
    #' @param epsilon epsilon which defines the explored range for the graph
    #' @param accuracy expected accuracy.
    #' @param num_edges number of edges for each node to explore the graph
    #' @param num_edges_for_search number of edges to search.
    #' @param batch_size batch size.
    refine_anng = function(epsilon,
                           accuracy,
                           num_edges,
                           num_edges_for_search,
                           batch_size) {
      private$check_index_opened()

      if (!is.numeric(c(epsilon, accuracy))) {
        rlang::abort("`epsilon` and `accuracy` must be numeric.")
      }
      if (!is.integer(c(num_edges, num_edges_for_search, batch_size))) {
        rlang::abort("`num_edges`, `num_edges_for_search` and `batch_size` must be integer.")
      }
      private$index$refine_anng(
        epsilon, accuracy, num_edges, num_edges_for_search, batch_size
      )
      invisible(self)
    },

    #' @description
    #' Gets objects from the index by their IDs.
    #' @param ids IDs of the objects to be retrieved.
    #' @return tibble.
    get_object = function(ids) {
      private$check_index_opened()

      ids <- as.integer(ids)
      tibble::as_tibble(
        private$index$get_object(ids),
        .name_repair = "unique"
      )
    },

    #' @description
    #' Resets the default parameters of the index.
    #' @param num_of_search_objects number of search objects.
    #' @param search_radius search radius.
    #' @param epsilon epsilon.
    #' @param edge_size edge size.
    #' @param expected_accuracy expected accuracy.
    reset_defaults = function(num_of_search_objects,
                              search_radius,
                              epsilon,
                              edge_size,
                              expected_accuracy) {
      private$check_index_opened()

      private$index$set(
        num_of_search_objects,
        search_radius,
        epsilon,
        edge_size,
        expected_accuracy
      )
      invisible(self)
    },

    #' @description
    #' Gets information of the index.
    #' @return named numeric vector
    get_info = function() {
      private$check_index_opened()

      c(
        num_dist_computations = private$index$get_num_of_distance_computations(),
        num_objects = private$index$get_num_of_objects(),
        repo_size = private$index$get_size_of_object_repository(),
        graph_size = private$index$get_size_of_graph_repository()
      )
    },

    #' @description
    #' Builds the search index.
    #' @param num_threads number of threads
    #' to be used for building a search index.
    #' @param target_size_of_graph target size of the graph.
    build_index = function(num_threads = 1L, target_size_of_graph = 0L) {
      private$check_index_opened()

      private$index$build_index(num_threads, target_size_of_graph)
      invisible(self)
    },

    #' @description
    #' Inserts data into the index and build a search index.
    #' @param data data to be inserted.
    #' @param num_threads number of threads to be used for building the index.
    #' @return the IDs of the inserted objects are returned invisibly.
    batch_insert = function(data, num_threads = 1L) {
      private$check_index_opened()

      if (!is.integer(num_threads) || num_threads < 1L) {
        rlang::abort("num_threads must be a positive integer.")
      }
      data <- as.matrix(data)
      invisible(
        private$index$batch_insert(data, num_threads)
      )
    },

    #' @description
    #' Inserts a vector to the index.
    #' To search with the index,
    #' you need to call `build_index` after call this method.
    #' @param vec vector to be inserted.
    #' @return the ID of the inserted object is returned invisibly.
    insert = function(vec) {
      private$check_index_opened()

      vec <- as.double(vec)
      invisible(
        private$index$insert(vec)
      )
    },

    #' @description
    #' Exports an index to a file.
    #' @param path path to save the index.
    export_index = function(path) {
      private$check_index_opened()

      private$index$export_index(path)
      invisible(self)
    },

    #' @description
    #' Imports an index from a file.
    #' @param path path to load the index.
    import_index = function(path) {
      private$check_index_opened()

      private$index$import_index(path)
      invisible(self)
    },

    #' @description
    #' Closes the index.
    close = function() {
      if (!is.null(private$index)) {
        private$index$close()
      }
      private$index <- NULL
      invisible(self)
    },

    #' @field path path of the index.
    path = NULL
  ),
  private = list(
    check_index_opened = function() {
      if (is.null(private$index)) {
        rlang::abort(
          paste(
            "There are no index opened.",
            "please open the index at first."
          )
        )
      }
      invisible()
    },
    index = NULL
  ),
  lock_objects = TRUE,
  portable = TRUE,
  cloneable = FALSE
)
