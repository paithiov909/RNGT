#include <Rcpp.h>

#include "NGT/lib/NGT/Index.h"
using namespace Rcpp;

// [[Rcpp::export]]
int rcpp_hello_world(std::string anng_path, std::string input_data) {
  NGT::Property property;
  property.dimension = 128;
  property.objectType = NGT::ObjectSpace::ObjectType::Float;
  property.distanceType = NGT::Index::Property::DistanceType::DistanceTypeL2;
  std::string path(anng_path);
  NGT::Index::create(path, property);
  NGT::Index index(path);
  std::ifstream is(input_data);
  std::string line;
  while (std::getline(is, line)) {
    std::stringstream linestream(line);
    std::vector<float> object;
    while (!linestream.eof()) {
      float value;
      linestream >> value;
      object.push_back(value);
    }
    object.resize(property.dimension);  // cut off unnecessary data in the file.
    index.append(object);
  }
  index.createIndex(16);  // 16 is the number of threads to build indexes.
  index.save();
  return 0;
}
