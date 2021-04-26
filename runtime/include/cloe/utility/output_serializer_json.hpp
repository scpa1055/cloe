/*
 * Copyright 2021 Robert Bosch GmbH
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
 * \file output_serializer_json.hpp
 */

#pragma once
#ifndef CLOE_UTILITY_OUTPUT_SERIALIZER_JSON_HPP_
#define CLOE_UTILITY_OUTPUT_SERIALIZER_JSON_HPP_

#include <string>  // for string

#include <cloe/core.hpp>                       // for Json
#include <cloe/utility/output_serializer.hpp>  // for Outputstream, JSONSerializer, ...

namespace cloe {
namespace utility {

enum FileTypeEnum {
  JSON_BZIP2,
  JSON_GZIP,
  JSON_ZIP,
  JSON,
};

// clang-format off
ENUM_SERIALIZATION(FileTypeEnum, ({
    {cloe::utility::FileTypeEnum::JSON_BZIP2   , "json.bz2"    },
    {cloe::utility::FileTypeEnum::JSON_GZIP    , "json.gz"     },
    {cloe::utility::FileTypeEnum::JSON_ZIP     , "json.zip"    },
    {cloe::utility::FileTypeEnum::JSON         , "json"        },
}))
// clang-format on

class SimpleJsonSerializer : public AbstractJsonSerializer<const Json&, bool> {
 public:
  typedef AbstractJsonSerializer<const Json&, bool> json_base;
  using json_base::json_base;
  virtual void serialize(const Json& j, bool write_delim) override {
    if (write_delim) {
      write(",\n");  // serialize delimiting comma, if already one dataset was serialized
    }
    auto txt = j.dump(3);  // serialize with level 3 indent
    write(txt);
  }
};

/// JsonFileSerializer is
/// 1) Interface for the consumer class
/// 2) the anchor point for exactly one instance of the default_filename
class JsonFileSerializer {
 public:
  virtual ~JsonFileSerializer() = 0;
  virtual void open_file(const std::string& filename) = 0;
  virtual void serialize(const Json& j, bool write_delim) = 0;
  virtual void close_file() = 0;

 protected:
  static const std::string default_filename;
};

/// JsonFileSerializerImpl is the implementation of JsonFileSerializer
template <typename TOutputStream>
class JsonFileSerializerImpl
    : public SequentialFileSerializer<SimpleJsonSerializer, TOutputStream, const Json&, bool>,
      public JsonFileSerializer {
  typedef SequentialFileSerializer<SimpleJsonSerializer, TOutputStream, const Json&, bool>
      file_base;

 public:
  JsonFileSerializerImpl(Logger& logger) : file_base(logger), JsonFileSerializer() {}
  virtual ~JsonFileSerializerImpl() override;
  using file_base::open_file;
  virtual void open_file(const std::string& filename) override {
    std::string default_name = this->outputstream_.make_default_filename(
        this->serializer_.make_default_filename(default_filename));
    file_base::open_file(filename, default_name);
  }
  virtual void serialize(const Json& j, bool write_delim) override {
    file_base::serialize(j, write_delim);
  }
  virtual void close_file() override { file_base::close_file(); }

 protected:
  virtual void on_file_opened() override { this->serializer_.start_array(); }
  virtual void on_file_closing() override { this->serializer_.end_array(); }
};

template <typename TOutputStream>
JsonFileSerializerImpl<TOutputStream>::~JsonFileSerializerImpl() {}

typedef JsonFileSerializerImpl<FileOutputStream> JsonSerializer;
typedef JsonFileSerializerImpl<ZlibOutputStream> ZlibJsonSerializer;
typedef JsonFileSerializerImpl<GzipOutputStream> GZipJsonSerializer;
typedef JsonFileSerializerImpl<Bzip2OutputStream> BZip2JsonSerializer;

std::unique_ptr<JsonFileSerializer> makeJsonFileSerializer(FileTypeEnum type, Logger log);

}  // namespace utility
}  // namespace cloe

#endif  // CLOE_UTILITY_OUTPUT_SERIALIZER_JSON_HPP_
