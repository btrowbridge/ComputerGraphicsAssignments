#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <DirectXMath.h>
#include <d3d11_2.h>

namespace Library
{
    class Material;
    class ModelMaterial;
	class OutputStreamHelper;
	class InputStreamHelper;

	struct MeshData
	{
		std::shared_ptr<ModelMaterial> Material;
		std::string Name;
		std::vector<DirectX::XMFLOAT3> Vertices;
		std::vector<DirectX::XMFLOAT3> Normals;
		std::vector<DirectX::XMFLOAT3> Tangents;
		std::vector<DirectX::XMFLOAT3> BiNormals;
		std::vector<std::vector<DirectX::XMFLOAT3>*> TextureCoordinates;
		std::vector<std::vector<DirectX::XMFLOAT4>*> VertexColors;
		std::uint32_t FaceCount;
		std::vector<std::uint32_t> Indices;

		MeshData();
		MeshData(const MeshData& rhs) = delete;
		MeshData& operator=(const MeshData& rhs) = delete;
		MeshData(MeshData&& rhs);
		MeshData& operator=(MeshData&& rhs);
		~MeshData();

	private:
		void Clear();
	};

    class Mesh
    {
    public:
		Mesh(Model& model, InputStreamHelper& streamHelper);
		Mesh(Model& model, MeshData&& meshData);
		Mesh(const Mesh& rhs) = delete;
		Mesh& operator=(const Mesh& rhs) = delete;
		Mesh(Mesh&& rhs);
		Mesh& operator=(Mesh&& rhs);

        Model& GetModel();
        std::shared_ptr<ModelMaterial> GetMaterial();
        const std::string& Name() const;

		const std::vector<DirectX::XMFLOAT3>& Vertices() const;
		const std::vector<DirectX::XMFLOAT3>& Normals() const;
		const std::vector<DirectX::XMFLOAT3>& Tangents() const;
		const std::vector<DirectX::XMFLOAT3>& BiNormals() const;
		const std::vector<std::vector<DirectX::XMFLOAT3>*>& TextureCoordinates() const;
		const std::vector<std::vector<DirectX::XMFLOAT4>*>& VertexColors() const;
		std::uint32_t FaceCount() const;
		const std::vector<std::uint32_t>& Indices() const;

        void CreateIndexBuffer(ID3D11Device& device, ID3D11Buffer** indexBuffer);
		void Save(OutputStreamHelper& streamHelper);

    private:
		void Load(InputStreamHelper& streamHelper);

        Model* mModel;
		MeshData mData;
    };
}