/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/*
 * reeds_shepp_path_test.cc
 */

#include <memory>

#include "gtest/gtest.h"
#include "modules/common/configs/proto/vehicle_config.pb.h"
#include "modules/common/configs/vehicle_config_helper.h"
#include "modules/common/util/util.h"
#include "modules/planning/common/planning_gflags.h"
#include "modules/planning/open_space/node3d.h"
#include "modules/planning/open_space/reeds_shepp_path.h"
#include "modules/planning/proto/planner_open_space_config.pb.h"

namespace apollo {
namespace planning {

class reeds_shepp : public ::testing::Test {
 public:
  virtual void SetUp() {
    ASSERT_TRUE(common::util::GetProtoFromFile(
        FLAGS_planner_open_space_config_filename, &open_space_conf_));
    vehicle_param_ = common::VehicleConfigHelper::GetConfig().vehicle_param();
    reedshepp_test = std::unique_ptr<ReedShepp>(
        new ReedShepp(vehicle_param_, open_space_conf_));
  }
  void check(std::shared_ptr<Node3d> start_node,
             std::shared_ptr<Node3d> end_node, ReedSheppPath* optimal_path) {
    ASSERT_LT(start_node->GetX() - (optimal_path->x).front(), 0.01);
    ASSERT_LT(start_node->GetY() - (optimal_path->y).front(), 0.01);
    ASSERT_LT(start_node->GetPhi() - (optimal_path->phi).front(), 0.01);
    ASSERT_LT(end_node->GetX() - (optimal_path->x).back(), 0.01);
    ASSERT_LT(end_node->GetY() - (optimal_path->y).back(), 0.01);
    ASSERT_LT(end_node->GetPhi() - (optimal_path->phi).back(), 0.01);
    ASSERT_GT(optimal_path->x.size(), 1);
    for (std::size_t i = 1; i < optimal_path->x.size(); i++) {
      double gold_interval =
          std::sqrt(open_space_conf_.warm_start_config().step_size() *
                        open_space_conf_.warm_start_config().step_size() +
                    open_space_conf_.warm_start_config().step_size() *
                        open_space_conf_.warm_start_config().step_size());
      double interval =
          std::sqrt((optimal_path->x.at(i) - optimal_path->x.at(i - 1)) *
                        (optimal_path->x.at(i) - optimal_path->x.at(i - 1)) +
                    (optimal_path->y.at(i) - optimal_path->y.at(i - 1)) *
                        (optimal_path->y.at(i) - optimal_path->y.at(i - 1)));
      ASSERT_LT(interval, gold_interval);
    }
  }

 protected:
  common::VehicleParam vehicle_param_;
  PlannerOpenSpaceConfig open_space_conf_;
  std::unique_ptr<ReedShepp> reedshepp_test;
};

TEST_F(reeds_shepp, test_set_1) {
  std::shared_ptr<Node3d> start_node = std::shared_ptr<Node3d>(
      new Node3d(0.0, 0.0, 10.0 * M_PI / 180.0, open_space_conf_));
  std::shared_ptr<Node3d> end_node = std::shared_ptr<Node3d>(
      new Node3d(7.0, -8.0, 50.0 * M_PI / 180.0, open_space_conf_));
  ReedSheppPath optimal_path;
  if (!reedshepp_test->ShortestRSP(start_node, end_node, &optimal_path)) {
    AINFO << "generating short RSP not successful";
  }
  check(start_node, end_node, &optimal_path);
}
TEST_F(reeds_shepp, test_set_2) {
  std::shared_ptr<Node3d> start_node = std::shared_ptr<Node3d>(
      new Node3d(0.0, 0.0, 10.0 * M_PI / 180.0, open_space_conf_));
  std::shared_ptr<Node3d> end_node = std::shared_ptr<Node3d>(
      new Node3d(7.0, -8.0, -50.0 * M_PI / 180.0, open_space_conf_));
  ReedSheppPath optimal_path;
  if (!reedshepp_test->ShortestRSP(start_node, end_node, &optimal_path)) {
    AINFO << "generating short RSP not successful";
  }
  check(start_node, end_node, &optimal_path);
}
TEST_F(reeds_shepp, test_set_3) {
  std::shared_ptr<Node3d> start_node = std::shared_ptr<Node3d>(
      new Node3d(0.0, 10.0, -10.0 * M_PI / 180.0, open_space_conf_));
  std::shared_ptr<Node3d> end_node = std::shared_ptr<Node3d>(
      new Node3d(-7.0, -8.0, -50.0 * M_PI / 180.0, open_space_conf_));
  ReedSheppPath optimal_path;
  if (!reedshepp_test->ShortestRSP(start_node, end_node, &optimal_path)) {
    AINFO << "generating short RSP not successful";
  }
  check(start_node, end_node, &optimal_path);
}
TEST_F(reeds_shepp, test_set_4) {
  std::shared_ptr<Node3d> start_node = std::shared_ptr<Node3d>(
      new Node3d(0.0, 10.0, -10.0 * M_PI / 180.0, open_space_conf_));
  std::shared_ptr<Node3d> end_node = std::shared_ptr<Node3d>(
      new Node3d(-7.0, -8.0, 150.0 * M_PI / 180.0, open_space_conf_));
  ReedSheppPath optimal_path;
  if (!reedshepp_test->ShortestRSP(start_node, end_node, &optimal_path)) {
    AINFO << "generating short RSP not successful";
  }
  check(start_node, end_node, &optimal_path);
}
TEST_F(reeds_shepp, test_set_5) {
  std::shared_ptr<Node3d> start_node = std::shared_ptr<Node3d>(
      new Node3d(0.0, 10.0, -10.0 * M_PI / 180.0, open_space_conf_));
  std::shared_ptr<Node3d> end_node = std::shared_ptr<Node3d>(
      new Node3d(7.0, 8.0, 150.0 * M_PI / 180.0, open_space_conf_));
  ReedSheppPath optimal_path;
  if (!reedshepp_test->ShortestRSP(start_node, end_node, &optimal_path)) {
    AINFO << "generating short RSP not successful";
  }
  check(start_node, end_node, &optimal_path);
}
}  // namespace planning
}  // namespace apollo
