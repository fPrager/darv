#include <VirtualObject.h>

// assimp include files. These three are usually needed.
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//#include <IL/il.h>
#include <igl/readOBJ.h>
#include <igl/readDMAT.h>
#include <igl/barycenter.h>
#include <igl/cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/grad.h>
#include <igl/doublearea.h>
#include <igl/repdiag.h>
#include <igl/per_vertex_normals.h>
#include <igl/igl_inline.h>
#include <igl/list_to_matrix.h>
#include <igl/matrix_to_list.h>
#include <igl/bounding_box_diagonal.h>
#include <igl/create_index_vbo.h>
#include <igl/create_vector_vbo.h>
#include <iostream>
#include <cstdio>
#include <fstream>

class OBJScene: public VirtualObject
{
	public:
		OBJScene(QString name, Vector3f* position, AngleAxisf* rotation, float sceneWidth = 1);
		~OBJScene();

		void Draw(bool useTracking = false);
		void Load();

	private:
		void ApplyMaterial(aiMaterial *mtl);
		
		bool LoadAsset(QString fileName);
		void Color4ToFloat4(aiColor4D *c, float f[4]);
		void SetFloat4(float f[4], float a, float b, float c, float d);

		int LoadGLTextures(const aiScene* scene);
		void RecursiveLoading(const aiScene *sc, aiNode* nd);
		void SmoothMesh();
		void DrawBuffer(int index, GLuint texture);

		void GetBoundingBox (aiVector3D* min, aiVector3D* max);
		void GetBoundingBoxForNode (const aiNode* nd, aiVector3D* min, aiVector3D* max, aiMatrix4x4* trafo);
		void SetModelSizes();

		Eigen::MatrixXd V,U;
		Eigen::MatrixXi F;
		Eigen::SparseMatrix<double> L;

		GLuint scene_list;
		QString fileName;
		Vector3f _boundMax;
		Vector3f _boundMin;
		float _sceneWidth;

		//buffers per mesh in obj-file
		QList<GLuint> _vertexbuffers;
		QList<GLuint> _indexbuffers;
		QList<int> _indexSizes;
		QList<GLenum> _faceModes;
		//QList<GLuint> _colorbuffers;
		QList<GLuint> _normalbuffers;
		QList<GLuint> _uvbuffers;
		QList<std::string> _textureIds;

		QMap<std::string, GLuint> _textureIdMap;	// map image filenames to textureIds
		
		const aiScene* scene;

};

