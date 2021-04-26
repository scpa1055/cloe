/*
 * Copyright 2020 Robert Bosch GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * \file cloe/utility/output_serializer.cpp
 * \see  cloe/utility/output_serializer.hpp
 */

#include <cloe/utility/output_serializer.hpp>

namespace cloe {
namespace utility {

const std::string AbstractJsonSerializerBase::json_array_open("\n[\n");   // NOLINT
const std::string AbstractJsonSerializerBase::json_array_close("\n]\n");  // NOLINT

bool BasicFileOutputStream::open_file(const std::string& filename,
                                      const std::string& default_filename) {
  const auto& output_file = filename == "" ? default_filename : filename;
  if (&output_file == &default_filename) {
    logger_->warn("No output file specified, using {}", output_file);
  }

  ofs_.open(output_file);
  bool success = !ofs_.fail();
  if (success) {
    logger_->info("Writing output to file: {}", output_file);
  } else {
    logger_->error("Error opening file for writing: {}", output_file);
  }
  return success;
}

void BasicFileOutputStream::close_stream() {
  if (ofs_.is_open()) {
    ofs_.close();
  }
}

/*AbstractJsonSerializer::~AbstractJsonSerializer() {}
std::string AbstractJsonSerializer::make_default_filename(const std::string& default_filename) {
  return default_filename + ".json";
}
void AbstractJsonSerializer::start_array() { base::write(json_array_open); }
void AbstractJsonSerializer::end_array() { base::write(json_array_close); }*/

}  // namespace utility
}  // namespace cloe
