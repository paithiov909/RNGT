#include "RNGT_types.h"

Index* create_index(const std::string path, std::size_t dimension,
                    int edgeSizeForCreation, int edgeSizeForSearch,
                    const std::string distanceType,
                    const std::string objectType) {
  Index::create(path, dimension, edgeSizeForCreation, edgeSizeForSearch,
                distanceType, objectType);
  return new Index(path, false, false, false);
}

void save_index(Index* obj, const std::string indexPath) {
#ifndef NGT_SHARED_MEMORY_ALLOCATOR
  obj->save(indexPath);
#else
  obj->save();
#endif
}

void close_index(Index* obj) {
  obj->close();
}

void build_index(Index* obj, std::size_t threadNumber, std::size_t sizeOfRepository) {
  obj->createIndex(threadNumber, sizeOfRepository);
}

std::size_t get_num_of_objects(Index* obj) {
  return obj->getNumberOfObjects();
}

std::size_t get_size_of_object_repository(Index* obj) {
  return obj->getObjectRepositorySize();
}

std::size_t get_size_of_graph_repository(Index* obj) {
  return obj->getGraphRepositorySize();
}

void export_index(Index* obj, const std::string ofile) {
  obj->exportIndex(ofile);
}

void import_index(Index* obj, const std::string ifile) {
  obj->importIndex(ifile);
}

RCPP_MODULE(RNGT) {
  using namespace Rcpp;

  Rcpp::class_<Index>("Index")
      // constructor
      .constructor<std::string, bool, bool, bool>()
      // create a new index and open it
      .factory<std::string, std::size_t, int, int, std::string, std::string>(create_index)

      .method("search", &Index::search)
      // .method("linear_search", &Index::linearSearch)
      .method("get_num_of_distance_computations", &Index::getNumOfDistanceComputations)
      .method("save", &save_index)
      .method("close", &close_index)
      .method("remove", &Index::remove)
      .method("build_index", &build_index)
      .method("get_num_of_objects", &get_num_of_objects)
      .method("get_size_of_object_repository", &get_size_of_object_repository)
      .method("get_size_of_graph_repository", &get_size_of_graph_repository)
      .method("get_object", &Index::getObject)
      .method("batch_insert", &Index::batchInsert)
      .method("insert", &Index::insert)
      .method("refine_anng", &Index::refineANNG)
      .method("set", &Index::set)
      .method("export_index", &export_index)
      .method("import_index", &import_index);
}
