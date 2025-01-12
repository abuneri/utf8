#include <gtest/gtest.h>

#include <auc/u8text.hpp>

#include "graphemebreaktest_data.hpp"

void PrintTo(const auc::codepoint& codepoint, ::std::ostream* os) {
  *os << codepoint.get_num();
}

class GraphemeBreakTest
    : public testing::TestWithParam<auc::detail::grapheme_cluster_break_test> {
};

void validate_clusters(
    const std::vector<auc::graphemecluster>& clusters,
    const std::vector<std::vector<auc::codepoint>>& expected_clusters) {
  std::vector<std::vector<auc::codepoint>> actual_clusters;
  for (const auto& cluster : clusters) {
    std::vector<auc::codepoint> codepoints;
    for (const auto& u8char : cluster.chars_) {
      codepoints.push_back(u8char.get_codepoint());
    }
    actual_clusters.push_back(codepoints);
  }

  EXPECT_EQ(expected_clusters, actual_clusters);
}

//=========================================================================
// Test all scenarios from:
// https://www.unicode.org/Public/15.0.0/ucd/auxiliary/GraphemeBreakTest.txt
//=========================================================================
TEST_P(GraphemeBreakTest, Clusters) {
  const auc::detail::grapheme_cluster_break_test& test = GetParam();

  const auto utf8 = auc::u8text::from_codepoints(test.codepoints_);

  const std::vector<auc::graphemecluster> clusters =
      utf8.get_grapheme_clusters();

  std::vector<std::vector<auc::codepoint>> actual_clusters;
  for (const auto& cluster : clusters) {
    std::vector<auc::codepoint> codepoints;
    for (const auto& u8char : cluster.chars_) {
      codepoints.push_back(u8char.get_codepoint());
    }
    actual_clusters.push_back(codepoints);
  }

  const auto& expected_clusters = test.clusters_;
  ASSERT_EQ(expected_clusters.size(), clusters.size());
  validate_clusters(clusters, expected_clusters);
}

INSTANTIATE_TEST_SUITE_P(
    BreakProperties,
    GraphemeBreakTest,
    testing::ValuesIn(auc::detail::grapheme_cluster_break_tests),
    [](const ::testing::TestParamInfo<auc::detail::grapheme_cluster_break_test>&
           info) { return info.param.name_; });

//=========================================================================
// Test all samples from:
// https://www.unicode.org/Public/15.0.0/ucd/auxiliary/GraphemeBreakTest.html#samples
//=========================================================================
TEST(grapheme_clusters, sample1) {
  char8_t utf8_chars[] = u8"\u000D\u000A\u0061\u000A\u0308";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(4u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x000Du}, auc::codepoint{0x000Au}},
                     {auc::codepoint{0x0061u}},
                     {auc::codepoint{0x000Au}},
                     {auc::codepoint{0x0308u}}});
}

TEST(grapheme_clusters, sample2) {
  char8_t utf8_chars[] = u8"\u0061\u0308";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(1u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0061u}, auc::codepoint{0x0308u}}});
}

TEST(grapheme_clusters, sample3) {
  char8_t utf8_chars[] = u8"\u0020\u200D\u0646";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0020u}, auc::codepoint{0x200Du}},
                     {auc::codepoint{0x0646u}}});
}

TEST(grapheme_clusters, sample4) {
  char8_t utf8_chars[] = u8"\u0646\u200D\u0020";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0646u}, auc::codepoint{0x200Du}},
                     {auc::codepoint{0x0020u}}});
}

TEST(grapheme_clusters, sample5) {
  char8_t utf8_chars[] = u8"\u1100\u1100";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(1u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x1100u}, auc::codepoint{0x1100u}}});
}

TEST(grapheme_clusters, sample6) {
  char8_t utf8_chars[] = u8"\uAC00\u11A8\u1100";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0xAC00u}, auc::codepoint{0x11A8u}},
                     {auc::codepoint{0x1100u}}});
}

TEST(grapheme_clusters, sample7) {
  char8_t utf8_chars[] = u8"\uAC01\u11A8\u1100";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0xAC01u}, auc::codepoint{0x11A8u}},
                     {auc::codepoint{0x1100u}}});
}

TEST(grapheme_clusters, sample8) {
  char8_t utf8_chars[] = u8"\U0001F1E6\U0001F1E7\U0001F1E8\u0062";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(3u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x1F1E6u}, auc::codepoint{0x1F1E7u}},
                     {auc::codepoint{0x1F1E8u}},
                     {auc::codepoint{0x0062u}}});
}

TEST(grapheme_clusters, sample9) {
  char8_t utf8_chars[] = u8"\u0061\U0001F1E6\U0001F1E7\U0001F1E8\u0062";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(4u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0061u}},
                     {auc::codepoint{0x1F1E6u}, auc::codepoint{0x1F1E7u}},
                     {auc::codepoint{0x1F1E8u}},
                     {auc::codepoint{0x0062u}}});
}

TEST(grapheme_clusters, sample10) {
  char8_t utf8_chars[] = u8"\u0061\U0001F1E6\U0001F1E7\u200D\U0001F1E8\u0062";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(4u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0061u}},
                     {auc::codepoint{0x1F1E6u}, auc::codepoint{0x1F1E7u},
                      auc::codepoint{0x200Du}},
                     {auc::codepoint{0x1F1E8u}},
                     {auc::codepoint{0x0062u}}});
}

TEST(grapheme_clusters, sample11) {
  char8_t utf8_chars[] = u8"\u0061\U0001F1E6\u200D\U0001F1E7\U0001F1E8\u0062";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(4u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0061u}},
                     {auc::codepoint{0x1F1E6u}, auc::codepoint{0x200Du}},
                     {auc::codepoint{0x1F1E7u}, auc::codepoint{0x1F1E8u}},
                     {auc::codepoint{0x0062u}}});
}

TEST(grapheme_clusters, sample12) {
  char8_t utf8_chars[] =
      u8"\u0061\U0001F1E6\U0001F1E7\U0001F1E8\U0001F1E9\u0062";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(4u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0061u}},
                     {auc::codepoint{0x1F1E6u}, auc::codepoint{0x1F1E7u}},
                     {auc::codepoint{0x1F1E8u}, auc::codepoint{0x1F1E9u}},
                     {auc::codepoint{0x0062u}}});
}

TEST(grapheme_clusters, sample13) {
  char8_t utf8_chars[] = u8"\u0061\u200D";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(1u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0061u}, auc::codepoint{0x200Du}}});
}

TEST(grapheme_clusters, sample14) {
  char8_t utf8_chars[] = u8"\u0061\u0308\u0062";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0061u}, auc::codepoint{0x0308u}},
                     {auc::codepoint{0x0062u}}});
}

TEST(grapheme_clusters, sample15) {
  char8_t utf8_chars[] = u8"\u0061\u0903\u0062";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0061u}, auc::codepoint{0x0903u}},
                     {auc::codepoint{0x0062u}}});
}

TEST(grapheme_clusters, sample16) {
  char8_t utf8_chars[] = u8"\u0061\u0600\u0062";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0061u}},
                     {auc::codepoint{0x0600u}, auc::codepoint{0x0062u}}});
}

TEST(grapheme_clusters, sample17) {
  char8_t utf8_chars[] = u8"\U0001F476\U0001F3FF\U0001F476";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x1F476u}, auc::codepoint{0x1F3FFu}},
                     {auc::codepoint{0x1F476u}}});
}

TEST(grapheme_clusters, sample18) {
  char8_t utf8_chars[] = u8"\u0061\U0001F3FF\U0001F476";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0061u}, auc::codepoint{0x1F3FFu}},
                     {auc::codepoint{0x1F476u}}});
}

TEST(grapheme_clusters, sample19) {
  char8_t utf8_chars[] = u8"\u0061\U0001F3FF\U0001F476\u200D\U0001F6D1";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0061u}, auc::codepoint{0x1F3FFu}},
                     {auc::codepoint{0x1F476u}, auc::codepoint{0x200Du},
                      auc::codepoint{0x1F6D1u}}});
}

TEST(grapheme_clusters, sample20) {
  char8_t utf8_chars[] =
      u8"\U0001F476\U0001F3FF\u0308\u200D\U0001F476\U0001F3FF";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(1u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x1F476u}, auc::codepoint{0x1F3FFu},
                      auc::codepoint{0x0308u}, auc::codepoint{0x200Du},
                      auc::codepoint{0x1F476u}, auc::codepoint{0x1F3FFu}}});
}

TEST(grapheme_clusters, sample21) {
  char8_t utf8_chars[] = u8"\U0001F6D1\u200D\U0001F6D1";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(1u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x1F6D1u}, auc::codepoint{0x200Du},
                      auc::codepoint{0x1F6D1u}}});
}

TEST(grapheme_clusters, sample22) {
  char8_t utf8_chars[] = u8"\u0061\u200D\U0001F6D1";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0061u}, auc::codepoint{0x200Du}},
                     {auc::codepoint{0x1F6D1u}}});
}

TEST(grapheme_clusters, sample23) {
  char8_t utf8_chars[] = u8"\u2701\u200D\u2701";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(1u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x2701u}, auc::codepoint{0x200Du},
                      auc::codepoint{0x2701u}}});
}

TEST(grapheme_clusters, sample24) {
  char8_t utf8_chars[] = u8"\u0061\u200D\u2701";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
  validate_clusters(clusters,
                    {{auc::codepoint{0x0061u}, auc::codepoint{0x200Du}},
                     {auc::codepoint{0x2701u}}});
}
