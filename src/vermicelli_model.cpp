/*!********************************************************************************************************************
 * @author  Ghassan Younes
 * @email   22338451+ghassanyounes\@users.noreply.github.com
 * @date    9/8/22
 * @brief
 * Copyright (c) 2022 Ghassan Younes. All rights reserved.
 *********************************************************************************************************************/

#define TINYOBJLOADER_IMPLEMENTATION

#include "vermicelli_model.h"
#include "vermicelli_functions.h"
#include <tiny_obj_loader.h>

#ifdef USE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>

#include <cassert>
#include <iostream>
#include <unordered_map>

namespace std {
template<>
struct hash<vermicelli::VermicelliModel::Vertex> {
    size_t operator()(vermicelli::VermicelliModel::Vertex const &vertex) const {
      size_t seed = 0;
      vermicelli::hashCombine(seed, vertex.mPosition, vertex.mColor, vertex.mNormal, vertex.mUV);
      return seed;
    }
};
}

namespace vermicelli {

VermicelliModel::VermicelliModel(VermicelliDevice &device, const VermicelliModel::Builder &builder, bool verbose)
        : mDevice{device}, mVerbose(verbose) {
  createVertexBuffers(builder.mVertices);
  createIndexBuffers(builder.mIndices);

}

VermicelliModel::~VermicelliModel() {
}

void VermicelliModel::bind(VkCommandBuffer commandBuffer) {
  VkBuffer     buffers[] = {mVertexBuffer->getBuffer()};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

  if (mHasIndexBuffer) {
    vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
  }
}

void VermicelliModel::draw(VkCommandBuffer commandBuffer) const {
  if (mHasIndexBuffer) {
    vkCmdDrawIndexed(commandBuffer, mIndexCount, 1, 0, 0, 0);
  } else {
    vkCmdDraw(commandBuffer, mVertexCount, 1, 0, 0);
  }
}

std::vector<VkVertexInputBindingDescription> VermicelliModel::Vertex::getBindingDescriptions() {
  return {{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
}

std::vector<VkVertexInputAttributeDescription> VermicelliModel::Vertex::getAttributeDescriptions() {
  return {
          {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, mPosition)}, // position
          {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, mColor)},     // color
          {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, mNormal)},     // color
          {3, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(Vertex, mUV)}     // color
  };
}

void VermicelliModel::createVertexBuffers(const std::vector<Vertex> &vertices) {
  mVertexCount = static_cast<uint32_t>(vertices.size());
  assert(mVertexCount >= 3 && "Vertex count must be >= 3");
  /// total number of bytes required for our vertex buffer to store all the vertices of the model
  VkDeviceSize bufferSize = sizeof(vertices[0]) * mVertexCount;
  uint32_t     vertexSize = sizeof(vertices[0]);

  VermicelliBuffer stagingBuffer{
          mDevice,
          vertexSize,
          mVertexCount,
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  };

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void *) vertices.data());

  mVertexBuffer = std::make_unique<VermicelliBuffer>(
          mDevice,
          vertexSize,
          mVertexCount,
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                                                    );

  mDevice.copyBuffer(stagingBuffer.getBuffer(), mVertexBuffer->getBuffer(), bufferSize);
}

void VermicelliModel::createIndexBuffers(const std::vector<uint32_t> &indices) {
  mIndexCount     = static_cast<uint32_t>(indices.size());
  mHasIndexBuffer = mIndexCount > 0;

  if (!mHasIndexBuffer) {
    return;
  }

  /// total number of bytes required for our vertex buffer to store all the vertices of the model
  VkDeviceSize     bufferSize = sizeof(indices[0]) * mIndexCount;
  uint32_t         indexSize  = sizeof(indices[0]);
  VermicelliBuffer stagingBuffer{
          mDevice,
          indexSize,
          mIndexCount,
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  };

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void *) indices.data());

  mIndexBuffer = std::make_unique<VermicelliBuffer>(
          mDevice,
          indexSize,
          mIndexCount,
          VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                                                   );

  mDevice.copyBuffer(stagingBuffer.getBuffer(), mIndexBuffer->getBuffer(), bufferSize);
}

std::unique_ptr<VermicelliModel>
VermicelliModel::createModelFromFile(VermicelliDevice &device, const std::string &filePath, const bool verbose) {
  Builder builder;
  builder.loadModel(filePath);

  if (verbose) {
    std::cout << "Vertex count for model " << filePath.substr(filePath.find_last_of('/') + 1) << ": "
              << builder.mVertices.size() << std::endl;
  }
  return std::make_unique<VermicelliModel>(device, builder, verbose);
}

#ifdef USE_ASSIMP

void VermicelliModel::Builder::loadModel(const std::string &filePath) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
  if (!scene || scene -> mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
    return;
  }
}
#else

void VermicelliModel::Builder::loadModel(const std::string &filePath) {
  tinyobj::ObjReaderConfig readr_config;
  readr_config.mtl_search_path = "../models/";

  tinyobj::ObjReader reader;
  if (!reader.ParseFromFile(filePath, readr_config)) {
    throw std::runtime_error("TinyObjReader: " + reader.Error());
  }
  if (!reader.Warning().empty()) {
    std::cout << "TinyObjReader warning: " << reader.Warning();
  }

  auto &attrib    = reader.GetAttrib();
  auto &shapes    = reader.GetShapes();
  auto &materials = reader.GetMaterials();

  mVertices.clear();
  mIndices.clear();

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};
  for (const auto                      &shape: shapes) {
    for (const auto &index: shape.mesh.indices) {
      Vertex vertex{};

      if (index.vertex_index >= 0) {
        vertex.mPosition = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
        };

        vertex.mColor = {
                attrib.colors[3 * index.vertex_index + 0],
                attrib.colors[3 * index.vertex_index + 1],
                attrib.colors[3 * index.vertex_index + 2]
        };

      }

      if (index.normal_index >= 0) {
        vertex.mNormal = {
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2]
        };
      }

      if (index.texcoord_index >= 0) {
        vertex.mUV = {
                attrib.texcoords[3 * index.texcoord_index + 0],
                attrib.texcoords[3 * index.texcoord_index + 1]
        };
      }

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(mVertices.size());
        mVertices.push_back(vertex);
      }
      mIndices.push_back(uniqueVertices[vertex]);
    }
    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
      shape.mesh.material_ids[f];
    }
  }
}

#endif
}

#if 0
// Example code to pull in mtl files. Refactor loadModel function to incorporate this sometime.

std::string inputfile = "cornell_box.obj";
tinyobj::ObjReaderConfig reader_config;
reader_config.mtl_search_path = "./"; // Path to material files

tinyobj::ObjReader reader;

if (!reader.ParseFromFile(inputfile, reader_config)) {
  if (!reader.Error().empty()) {
      std::cerr << "TinyObjReader: " << reader.Error();
  }
  exit(1);
}

if (!reader.Warning().empty()) {
  std::cout << "TinyObjReader: " << reader.Warning();
}

auto& attrib = reader.GetAttrib();
auto& shapes = reader.GetShapes();
auto& materials = reader.GetMaterials();

// Loop over shapes
for (size_t s = 0; s < shapes.size(); s++) {
  // Loop over faces(polygon)
  size_t index_offset = 0;
  for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
    size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

    // Loop over vertices in the face.
    for (size_t v = 0; v < fv; v++) {
      // access to vertex
      tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
      tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
      tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
      tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

      // Check if `normal_index` is zero or positive. negative = no normal data
      if (idx.normal_index >= 0) {
        tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
        tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
        tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
      }

      // Check if `texcoord_index` is zero or positive. negative = no texcoord data
      if (idx.texcoord_index >= 0) {
        tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
        tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
      }

      // Optional: vertex colors
      // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
      // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
      // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
    }
    index_offset += fv;

    // per-face material
    shapes[s].mesh.material_ids[f];
  }
}

#endif