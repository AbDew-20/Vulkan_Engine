#include "ve_vertex.h"
#include <stdexcept>
namespace ve {
   
    

    VertexBuffer::VertexBuffer(VeDevice& device, std::vector<Vertex> &vertices, std::vector<uint16_t> &indices) :veDevice{ device } {
        createVertexBuffer(vertices);
        createIndexBuffer(indices);
        vertNums = static_cast<uint32_t>(indices.size());
    }

    VertexBuffer::~VertexBuffer() {
        vkDestroyBuffer(veDevice.device(), indexBuffer, nullptr);
        vkFreeMemory(veDevice.device(), indexBufferMemory, nullptr);

        vkDestroyBuffer(veDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(veDevice.device(),vertexBufferMemory,nullptr);
    }

    void VertexBuffer::createVertexBuffer(std::vector<Vertex>& vertices) {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        veDevice.createBuffer(bufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,stagingBuffer,stagingBufferMemory);
        veDevice.fillBuffer(stagingBufferMemory, 0, bufferSize, 0, vertices.data());

        veDevice.createBuffer(bufferSize,VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,vertexBuffer,vertexBufferMemory);
        veDevice.copyBuffer(stagingBuffer,vertexBuffer,bufferSize);

        vkDestroyBuffer(veDevice.device(), stagingBuffer, nullptr);
        vkFreeMemory(veDevice.device(), stagingBufferMemory, nullptr);
    }
    void VertexBuffer::createIndexBuffer(std::vector<uint16_t>& indices) {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        veDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        veDevice.fillBuffer(stagingBufferMemory, 0, bufferSize, 0, indices.data());

        veDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

        veDevice.copyBuffer(stagingBuffer,indexBuffer,bufferSize);
        vkDestroyBuffer(veDevice.device(),stagingBuffer,nullptr);
        vkFreeMemory(veDevice.device(), stagingBufferMemory, nullptr);


    }
    
    

}