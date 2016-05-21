#ifndef HMM2D_H
#define HMM2D_H
#include <Eigen/Dense>
#include <map>
#include <vector>
#include <iostream>
#include "pixelizer.h"

typedef Eigen::Matrix<long double, Eigen::Dynamic, Eigen::Dynamic> MatrixXd;
typedef std::vector<long double> VectorXd;

class HMM2D {
  static void VectorNormalize(VectorXd &v) {
    long double sum = 0;
    for (size_t i = 0; i < v.size(); ++i) {
      sum += v[i];
    }
    for (size_t i = 0; i < v.size(); ++i) {
      v[i] /= sum;
    }
  }
  static void MatrixRowNormalize(MatrixXd &a) {
    for (size_t i = 0; i < a.rows(); ++i) {
      long double sum = 0;
      for (size_t j = 0; j < a.cols(); ++j) {
        sum += a(i, j);
      }
      if (sum) for (size_t j = 0; j < a.cols(); ++j) {
        a(i, j) /= sum;
      }
    }
  }
  void RowNormalize() {
    VectorNormalize(pix);
    VectorNormalize(piy);
    MatrixRowNormalize(ax);
    MatrixRowNormalize(ay);
  }
 public:
  enum class Direction { X, Y };
  typedef size_t Observation;
  typedef size_t State;
  std::map<State, size_t> state_map;
  std::vector<State> states;
  std::vector<Observation> observations;
  MatrixXd ax;
  MatrixXd ay;
  VectorXd pix;
  VectorXd piy;
  template <int format> static HMM2D *FromPNG(PNG<format> *png) {
    HMM2D *retval = new HMM2D();
    State last_depth;
    State depth;
    std::vector<State> initial;
    std::vector<std::pair<State, State>> transitions;
    typename PNG<format>::Pixel *pixels = png->GetPixelArray();
    bool init = true;
    for (size_t i = 0; i < png->GetWidth(); ++i) {
      size_t sum = 0;
      for (size_t j = 0; j < png->GetHeight(); ++j) {
        if (init) {
          initial.push_back((depth = pixels[i*png->GetHeight() + j].GetValue()));
          init = false;
        } else {
          transitions.push_back(std::pair<State, State>(last_depth, (depth = pixels[i*png->GetHeight() + j].GetValue())));
        }
        sum += depth;
        last_depth = depth;
        retval->states.push_back(depth);
      }
      retval->observations.push_back(sum);
      init = true;
    }
    std::sort(retval->states.begin(), retval->states.end());
    auto it = std::unique(retval->states.begin(), retval->states.end());
    retval->states.resize(std::distance(retval->states.begin(), it));
    for (auto it = retval->states.begin(); it != retval->states.end(); it++) {
      retval->state_map[*it] = std::distance(retval->states.begin(), it);
    }
    retval->pix.resize(retval->states.size());
    retval->piy.resize(retval->states.size());
    retval->ax.resize(retval->states.size(), retval->states.size());
    retval->ay.resize(retval->states.size(), retval->states.size());
    for (auto it = initial.begin(); it != initial.end(); it++) {
      retval->pix[retval->state_map[*it]]++;
    }
    for (auto it = transitions.begin(); it != transitions.end(); it++) {
      retval->ax(retval->state_map[it->first], retval->state_map[it->second])++;
    }
    initial.clear();
    transitions.clear();
    for (size_t j = 0; j < png->GetHeight(); ++j) {
      for (size_t i = 0; i < png->GetWidth(); ++i) {
        if (init) {
          initial.push_back((depth = pixels[i*png->GetHeight() + j].GetValue()));
          init = false;
        } else {
          transitions.push_back(std::pair<State, State>(last_depth, (depth = pixels[i*png->GetHeight() + j].GetValue())));
        }
        last_depth = depth;
      }
      init = true;
    }
    for (auto it = initial.begin(); it != initial.end(); it++) {
      retval->piy[retval->state_map[*it]]++;
    }
    for (auto it = transitions.begin(); it != transitions.end(); it++) {
      retval->ay(retval->state_map[it->first], retval->state_map[it->second])++;
    }
    retval->RowNormalize();
    return retval;
  }
  void PrintObs() {
    std::cout << "[ ";
    for (auto it = observations.cbegin(); it != observations.cend(); it++) {
      std::cout << *it << " ";
    }
    std::cout << "]" << std::endl;
  }
  void Print() {
    std::cout << "[ ";
    for (auto it = pix.begin(); it != pix.end(); it++) {
      std::cout << *it << " ";
    }
    std::cout << "]" << std::endl;
    std::cout << "[ ";
    for (auto it = piy.begin(); it != piy.end(); it++) {
      std::cout << *it << " ";
    }
    std::cout << "]" << std::endl;
  }
};
    
#endif
