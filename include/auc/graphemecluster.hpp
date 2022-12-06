#pragma once

#include <vector>

#include "u8char.hpp"

namespace auc {

struct graphemecluster;

namespace detail {

std::vector<graphemecluster> build_grapheme_cluters(
    const std::vector<u8char>& chars);

}  // namespace detail

struct graphemecluster {
  std::vector<u8char> chars_;
};

}  // namespace auc
