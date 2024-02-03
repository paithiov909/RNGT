#include <Rcpp.h>

#include "NGT/lib/NGT/Index.h"
#include "NGT/lib/NGT/GraphOptimizer.h"

using namespace Rcpp;

class Index : public NGT::Index {
 public:
  Index(std::string path, bool readOnly, bool treeDisabled, bool logDisabled)
      : NGT::Index(path, readOnly) {
    if (logDisabled) {
      NGT::Index::disableLog();
    } else {
      NGT::Index::enableLog();
    }
    numOfDistanceComputations = 0;
    treeIndex = !treeDisabled;
    defaultNumOfSearchObjects = 20;
    defaultEpsilon = 0.1;
    defaultRadius = FLT_MAX;
    defaultEdgeSize = -1;
    defaultExpectedAccuracy = -1.0;
  }

  static void create(const std::string path, std::size_t dimension,
                     int edgeSizeForCreation, int edgeSizeForSearch,
                     const std::string distanceType,
                     const std::string objectType) {
    NGT::Property prop;
    prop.dimension = dimension;
    prop.edgeSizeForCreation = edgeSizeForCreation;
    prop.edgeSizeForSearch = edgeSizeForSearch;

    if (objectType == "Float" || objectType == "float") {
      prop.objectType = NGT::ObjectSpace::ObjectType::Float;
    } else if (objectType == "Byte" || objectType == "byte") {
      prop.objectType = NGT::ObjectSpace::ObjectType::Uint8;
#ifdef NGT_HALF_FLOAT
    } else if (objectType == "Float16" || objectType == "float16") {
      prop.objectType = NGT::ObjectSpace::ObjectType::Float16;
#endif
    } else {
      Rcpp::stop("Invalid object type.");
    }

    if (distanceType == "L1") {
      prop.distanceType = NGT::Index::Property::DistanceType::DistanceTypeL1;
    } else if (distanceType == "L2") {
      prop.distanceType = NGT::Property::DistanceType::DistanceTypeL2;
    } else if (distanceType == "Normalized L2") {
      prop.distanceType = NGT::Property::DistanceType::DistanceTypeNormalizedL2;
    } else if (distanceType == "Hamming") {
      prop.distanceType = NGT::Property::DistanceType::DistanceTypeHamming;
    } else if (distanceType == "Jaccard") {
      prop.distanceType = NGT::Property::DistanceType::DistanceTypeJaccard;
    } else if (distanceType == "Sparse Jaccard") {
      prop.distanceType =
          NGT::Property::DistanceType::DistanceTypeSparseJaccard;
    } else if (distanceType == "Angle") {
      prop.distanceType = NGT::Property::DistanceType::DistanceTypeAngle;
    } else if (distanceType == "Normalized Angle") {
      prop.distanceType =
          NGT::Property::DistanceType::DistanceTypeNormalizedAngle;
    } else if (distanceType == "Cosine") {
      prop.distanceType = NGT::Property::DistanceType::DistanceTypeCosine;
    } else if (distanceType == "Normalized Cosine") {
      prop.distanceType =
          NGT::Property::DistanceType::DistanceTypeNormalizedCosine;
    } else if (distanceType == "Normalized L2") {
      prop.distanceType = NGT::Property::DistanceType::DistanceTypeNormalizedL2;
    } else {
      Rcpp::stop("Invalid distance type.");
    }
    NGT::Index::createGraphAndTree(path, prop);
  }

  std::vector<int> batchInsert(Rcpp::NumericMatrix data,
                               std::size_t numThreads = 1) {
    NGT::Property prop;
    NGT::Index::getProperty(prop);
    if (prop.dimension != data.ncol()) {
      Rcpp::stop("Dimensions are inconsistent.");
    }
    std::size_t dataSize = data.nrow();
    std::vector<int> ids;
    ids.reserve(dataSize);

    Rcpp::DoubleVector r;
    std::vector<double> obj;
    for (std::size_t i = 0; i < dataSize; i++) {
      r = data.row(i);
      if (Rcpp::any(Rcpp::is_na(r)) | Rcpp::any(Rcpp::is_nan(r)) |
          Rcpp::any(Rcpp::is_infinite(r))) {
        Rprintf("Row %d has invalid values, skipped.", i);
        continue;
      }
      obj = Rcpp::as<std::vector<double>>(r);
      ids.push_back(NGT::Index::insert(obj));
    }
    NGT::Index::createIndex(numThreads);
    numOfDistanceComputations = 0;

    return ids;
  }

  int insert(std::vector<double> data, bool debug = false) {
    int id = NGT::Index::insert(data);
    if (debug) {
      Rcpp::Rcout << "id: " << id << "\n";
    }
    numOfDistanceComputations = 0;
    return id;
  }

  Rcpp::DataFrame search(const std::vector<double> query, std::size_t size,
                         float epsilon, int edgeSize, float expectedAccuracy,
                         bool withDistance = true) {
    NGT::Object* ngtquery = 0;
    ngtquery = NGT::Index::allocateObject(query);
    NGT::SearchContainer sc(*ngtquery);
    sc.setSize(size == 0 ? defaultNumOfSearchObjects : size);
    sc.setRadius(defaultRadius);
    if (expectedAccuracy > 0.0) {
      sc.setExpectedAccuracy(expectedAccuracy);
    } else {
      sc.setEpsilon(epsilon <= -1.0 ? defaultEpsilon : epsilon);
    }
    sc.setEdgeSize(edgeSize < -2 ? defaultEdgeSize : edgeSize);

    if (treeIndex) {
      NGT::Index::search(sc);
    } else {
      NGT::Index::searchUsingOnlyGraph(sc);
    }
    numOfDistanceComputations += sc.distanceComputationCount;

    NGT::Index::deleteObject(ngtquery);
    if (!withDistance) {
      NGT::ResultPriorityQueue& r = sc.getWorkingResult();
      std::vector<int> ids;
      ids.reserve(r.size());
      for (std::size_t i = 0; i < r.size(); ++i) {
        ids.push_back(r.top().id);
        r.pop();
      }
      return Rcpp::DataFrame::create(Rcpp::Named("id") = Rcpp::wrap(ids));
    } else {
      NGT::ObjectDistances r;
      r.moveFrom(sc.getWorkingResult());
      std::vector<int> ids;
      std::vector<double> distances;
      ids.reserve(r.size());
      distances.reserve(r.size());
      for (auto ri = r.begin(); ri != r.end(); ++ri) {
        ids.push_back((*ri).id);
        distances.push_back((*ri).distance);
      }
      return Rcpp::DataFrame::create(
          Rcpp::Named("id") = Rcpp::wrap(ids),
          Rcpp::Named("distance") = Rcpp::wrap(distances));
    }
  }

  // TODO: linearSearch;


  void remove(const std::vector<int> ids) {
    for (auto id : ids) {
      NGT::Index::remove(id);
    }
  }

  void refineANNG(float epsilon, float accuracy,
                  int numOfEdges, int numOfExploredEdges,
                  std::size_t batchSize) {
    bool unlog = NGT::Index::redirect;
    NGT::GraphReconstructor::refineANNG(*this, unlog,
                                        epsilon, accuracy,
                                        numOfEdges, numOfExploredEdges,
                                        batchSize);
  }

  Rcpp::NumericMatrix getObject(const std::vector<int> ids) {
    NGT::Property prop;
    NGT::Index::getProperty(prop);
    std::vector<float> object;
    object.reserve(prop.dimension * ids.size());
    for (auto id : ids) {
      switch (prop.objectType) {
        case NGT::ObjectSpace::ObjectType::Uint8: {
          auto* obj =
              static_cast<uint8_t*>(NGT::Index::getObjectSpace().getObject(id));
          for (int i = 0; i < prop.dimension; i++) {
            object.push_back(*obj++);
          }
          break;
        }
        default:
        case NGT::ObjectSpace::ObjectType::Float: {
          auto* obj =
              static_cast<float*>(NGT::Index::getObjectSpace().getObject(id));
          for (int i = 0; i < prop.dimension; i++) {
            object.push_back(*obj++);
          }
          break;
        }
      }
    }
    Rcpp::NumericVector vec = Rcpp::wrap(object);
    vec.attr("dim") = Rcpp::Dimension(ids.size(), prop.dimension);
    return Rcpp::as<Rcpp::NumericMatrix>(vec);
  }

  void set(std::size_t numOfSearchObjects, NGT::Distance radius, float epsilon,
           int edgeSize, float expectedAccuracy) {
    defaultNumOfSearchObjects =
        numOfSearchObjects > 0 ? numOfSearchObjects : defaultNumOfSearchObjects;
    defaultEpsilon = epsilon > -1.0 ? epsilon : defaultEpsilon;
    defaultRadius = radius >= 0.0 ? radius : defaultRadius;
    defaultEdgeSize = edgeSize >= -2 ? edgeSize : defaultEdgeSize;
    defaultExpectedAccuracy =
        expectedAccuracy > 0.0 ? expectedAccuracy : defaultExpectedAccuracy;
  }

  std::size_t getNumOfDistanceComputations() {
    return numOfDistanceComputations;
  }

  std::size_t numOfDistanceComputations;
  std::size_t numOfSearchObjects;
  bool treeIndex;
  std::size_t defaultNumOfSearchObjects;
  float defaultEpsilon;
  float defaultRadius;
  std::int64_t defaultEdgeSize;
  float defaultExpectedAccuracy;
};
