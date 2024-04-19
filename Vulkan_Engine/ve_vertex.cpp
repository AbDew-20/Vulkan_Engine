#include "ve_vertex.h"
#include <stdexcept>
namespace ve {
   
    

    VertexBuffer::VertexBuffer(VeDevice& device, std::vector<Vertex> &vertices, std::vector<uint16_t> &indices) :veDevice{ device } {
        createVertexBuffer(vertices);
        createIndexBuffer(indices);
        vertNums = static_cast<uint32_t>(indices.size());
    }

    VertexBuffer::~VertexBuffer() {
        veDevice.destroyBuffer(vertexBuffer,vertexAllocation);
        veDevice.destroyBuffer(indexBuffer, indexAllocation);


    }

    void VertexBuffer::createVertexBuffer(std::vector<Vertex>& vertices) {
       
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferMemory;
        (void)veDevice.createBuffer(bufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,stagingBuffer,stagingBufferMemory);
        veDevice.fillBuffer(stagingBufferMemory,0,bufferSize,vertices.data());

        (void)veDevice.createBuffer(bufferSize,VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT,0,vertexBuffer,vertexAllocation);
        veDevice.copyBuffer(stagingBuffer,vertexBuffer,bufferSize);

        veDevice.destroyBuffer(stagingBuffer,stagingBufferMemory);
        
    }
    void VertexBuffer::createIndexBuffer(std::vector<uint16_t>& indices) {
        

        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
        VkBuffer stagingBuffer;
        VmaAllocation stagingBufferMemory;
        (void)veDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, stagingBuffer, stagingBufferMemory);
        veDevice.fillBuffer(stagingBufferMemory, 0, bufferSize, indices.data());

        (void)veDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0, indexBuffer, indexAllocation);
        veDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        veDevice.destroyBuffer(stagingBuffer, stagingBufferMemory);


    }
    
    

}