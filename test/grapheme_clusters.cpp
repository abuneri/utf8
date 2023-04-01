#include <gtest/gtest.h>
#include <auc/u8text.hpp>
#include "graphemebreaktest_data.hpp"

// TODO: validate all samples from:
// https://www.unicode.org/Public/15.0.0/ucd/auxiliary/GraphemeBreakTest.html#samples

// TODO: Properly test all scnaerios from:
// https://www.unicode.org/Public/15.0.0/ucd/auxiliary/GraphemeBreakTest.txt
// via gtest TEST_P on data structure generated from python script parsing the above file

TEST(grapheme_clusters, sample1) {
  char utf8_chars[] = u8"\u000D\u000A\u0061\u000A\u0308";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(4u, clusters.size());
}

TEST(grapheme_clusters, sample2) {
  char utf8_chars[] = u8"\u0061\u0308";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(1u, clusters.size());
}

TEST(grapheme_clusters, sample3) {
  char utf8_chars[] = u8"\u0020\u200D\u0646";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
}

TEST(grapheme_clusters, sample4) {
  char utf8_chars[] = u8"\u0646\u200D\u0020";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
}

TEST(grapheme_clusters, sample5) {
  char utf8_chars[] = u8"\u1100\u1100";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(1u, clusters.size());
}

TEST(grapheme_clusters, sample6) {
  char utf8_chars[] = u8"\uAC00\u11A8\u1100";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
}

TEST(grapheme_clusters, sample7) {
  char utf8_chars[] = u8"\uAC01\u11A8\u1100";
  auc::u8text utf8_text(utf8_chars);

  const std::vector<auc::graphemecluster> clusters =
      utf8_text.get_grapheme_clusters();
  ASSERT_EQ(2u, clusters.size());
}

// TODO: Support specialized regional indicator behaviour
//TEST(grapheme_clusters, sample8) {
//  char utf8_chars[] = u8"\U0001F1E6\U0001F1E7\U0001F1E8\u0062";
//  auc::u8text utf8_text(utf8_chars);
//
//  const std::vector<auc::graphemecluster> clusters =
//      utf8_text.get_grapheme_clusters();
//  ASSERT_EQ(3u, clusters.size());
//}
//
// TEST(grapheme_clusters, sample9) {
//   char utf8_chars[] = u8"\u0061\U0001F1E6\U0001F1E7\U0001F1E8\u0062";
//   auc::u8text utf8_text(utf8_chars);
//
//   const std::vector<auc::graphemecluster> clusters =
//       utf8_text.get_grapheme_clusters();
//   ASSERT_EQ(1u, clusters.size());
// }
//
// TEST(grapheme_clusters, sample10) {
//   char utf8_chars[] = u8"";
//   auc::u8text utf8_text(utf8_chars);
//
//   const std::vector<auc::graphemecluster> clusters =
//       utf8_text.get_grapheme_clusters();
//   ASSERT_EQ(1u, clusters.size());
// }
//
// TEST(grapheme_clusters, sample11) {
//   char utf8_chars[] = u8"";
//   auc::u8text utf8_text(utf8_chars);
//
//   const std::vector<auc::graphemecluster> clusters =
//       utf8_text.get_grapheme_clusters();
//   ASSERT_EQ(1u, clusters.size());
// }
//
// TEST(grapheme_clusters, sample12) {
//   char utf8_chars[] = u8"";
//   auc::u8text utf8_text(utf8_chars);
//
//   const std::vector<auc::graphemecluster> clusters =
//       utf8_text.get_grapheme_clusters();
//   ASSERT_EQ(1u, clusters.size());
// }

 TEST(grapheme_clusters, sample13) {
   char utf8_chars[] = u8"\u0061\u200D";
   auc::u8text utf8_text(utf8_chars);

   const std::vector<auc::graphemecluster> clusters =
       utf8_text.get_grapheme_clusters();
   ASSERT_EQ(1u, clusters.size());
 }

 TEST(grapheme_clusters, sample14) {
   char utf8_chars[] = u8"\u0061\u0308\u0062";
   auc::u8text utf8_text(utf8_chars);

   const std::vector<auc::graphemecluster> clusters =
       utf8_text.get_grapheme_clusters();
   ASSERT_EQ(2u, clusters.size());
}

// TODO: SpacingMark tests
//TEST(grapheme_clusters, sample15) {
//  char utf8_chars[] = u8"";
//  auc::u8text utf8_text(utf8_chars);
//
//  const std::vector<auc::graphemecluster> clusters =
//      utf8_text.get_grapheme_clusters();
//  ASSERT_EQ(1u, clusters.size());
//}

 // TODO: Fix prepend bug
//TEST(grapheme_clusters, sample16) {
//  char utf8_chars[] = u8"\u0061\u0600\u0062";
//  auc::u8text utf8_text(utf8_chars);
//
//  const std::vector<auc::graphemecluster> clusters =
//      utf8_text.get_grapheme_clusters();
//  ASSERT_EQ(2u, clusters.size());
//}

// TODO: Support ExtPict grapheme break property
// TEST(grapheme_clusters, sample17) {
//   char utf8_chars[] = u8"\U0001F476\U0001F3FF";
//   auc::u8text utf8_text(utf8_chars);
//
//   const std::vector<auc::graphemecluster> clusters =
//       utf8_text.get_grapheme_clusters();
//   ASSERT_EQ(1u, clusters.size());
// }
//
// TEST(grapheme_clusters, sample18) {
//   char utf8_chars[] = u8"";
//   auc::u8text utf8_text(utf8_chars);
//
//   const std::vector<auc::graphemecluster> clusters =
//       utf8_text.get_grapheme_clusters();
//   ASSERT_EQ(1u, clusters.size());
// }
//
// TEST(grapheme_clusters, sample19) {
//   char utf8_chars[] = u8"";
//   auc::u8text utf8_text(utf8_chars);
//
//   const std::vector<auc::graphemecluster> clusters =
//       utf8_text.get_grapheme_clusters();
//   ASSERT_EQ(1u, clusters.size());
// }
//
// TEST(grapheme_clusters, sample20) {
//   char utf8_chars[] = u8"";
//   auc::u8text utf8_text(utf8_chars);
//
//   const std::vector<auc::graphemecluster> clusters =
//       utf8_text.get_grapheme_clusters();
//   ASSERT_EQ(1u, clusters.size());
// }
//
// TEST(grapheme_clusters, sample21) {
//   char utf8_chars[] = u8"";
//   auc::u8text utf8_text(utf8_chars);
//
//   const std::vector<auc::graphemecluster> clusters =
//       utf8_text.get_grapheme_clusters();
//   ASSERT_EQ(1u, clusters.size());
// }
//
// TEST(grapheme_clusters, sample22) {
//   char utf8_chars[] = u8"";
//   auc::u8text utf8_text(utf8_chars);
//
//   const std::vector<auc::graphemecluster> clusters =
//       utf8_text.get_grapheme_clusters();
//   ASSERT_EQ(1u, clusters.size());
// }
//
// TEST(grapheme_clusters, sample23) {
//  char utf8_chars[] = u8"\u2701\u200D\u2701";
//  auc::u8text utf8_text(utf8_chars);
//
//  const std::vector<auc::graphemecluster> clusters =
//      utf8_text.get_grapheme_clusters();
//  ASSERT_EQ(1u, clusters.size());
//}
//
// TEST(grapheme_clusters, sample24) {
//   char utf8_chars[] = u8"";
//   auc::u8text utf8_text(utf8_chars);
//
//   const std::vector<auc::graphemecluster> clusters =
//       utf8_text.get_grapheme_clusters();
//   ASSERT_EQ(1u, clusters.size());
// }
