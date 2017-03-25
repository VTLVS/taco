#ifndef TACO_UTIL_FILL_H
#define TACO_UTIL_FILL_H

#include <string>
#include <random>

namespace taco {
namespace util {

enum class FillMethod {
  Dense,
  Sparse,
  Slicing,
  FEM,
  HyperSpace
};

const std::map<FillMethod,double> fillFactors = {
    {FillMethod::Dense, 0.95},
    {FillMethod::Sparse, 0.07},
    {FillMethod::HyperSpace, 0.005}
};

const double doubleLowerBound = -10e6;
const double doubleUpperBound =  10e6;

void fillTensor(TensorBase& tens, const FillMethod& fill);
void fillVector(TensorBase& tens, const FillMethod& fill);
void fillMatrix(TensorBase& tens, const FillMethod& fill);

void fillTensor(TensorBase& tens, const FillMethod& fill) {
  switch (tens.getOrder()) {
    case 1: {
      fillVector(tens, fill);
      break;
    }
    case 2: {
      fillMatrix(tens, fill);
      break;
    }
    default:
      taco_uerror << "Impossible to fill tensor " << tens.getName() <<
        " of dimension " << tens.getOrder() << std::endl;
  }
}

void fillVector(TensorBase& tens, const FillMethod& fill) {
  // Random values
  std::uniform_real_distribution<double> unif(doubleLowerBound,
                                              doubleUpperBound);
  std::default_random_engine re;
  re.seed(std::random_device{}());
  int vectorSize=tens.getDimensions()[0];
  // Random positions
  std::vector<int> positions(vectorSize);
  for (int i=0; i<vectorSize; i++)
    positions.push_back(i);
  srand(time(0));
  std::random_shuffle(positions.begin(),positions.end());
  switch (fill) {
    case FillMethod::Dense:
    case FillMethod::Sparse:
    case FillMethod::HyperSpace: {
      int toFill=fillFactors.at(fill)*vectorSize;
      for (int i=0; i<toFill; i++) {
        tens.insert({positions[i]}, unif(re));
      }
      tens.pack();
      break;
    }
    default: {
      taco_uerror << "FillMethod not available for vectors" << std::endl;
      break;
    }
  }
}

void fillMatrix(TensorBase& tens, const FillMethod& fill) {
  switch (fill) {
    case FillMethod::Dense: {
      std::uniform_real_distribution<double> unif(doubleLowerBound,
                                                  doubleUpperBound);
      std::default_random_engine re;
      re.seed(std::random_device{}());
      for (int i=0; i<(fillFactors.at(fill)*tens.getDimensions()[0]); i++) {
        for (int j=0; j<(fillFactors.at(fill)*tens.getDimensions()[1]); j++) {
          tens.insert({i,j}, unif(re));
        }
      }
      tens.pack();
      break;
    }
    case FillMethod::Slicing:
    case FillMethod::FEM:
    case FillMethod::HyperSpace: {
      taco_not_supported_yet;
      break;
    }
    default: {
      taco_uerror << "FillMethod not available for matrices" << std::endl;
      break;
    }
  }

}

}}
#endif
