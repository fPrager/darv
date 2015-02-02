#include "OBJScene.h"
#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)
// ----------------------------------------------------------------------------
void OBJScene::GetBoundingBoxForNode (const aiNode* nd, aiVector3D* min, aiVector3D* max, aiMatrix4x4* trafo){
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	prev = *trafo;
	aiMultiplyMatrix4(trafo,&nd->mTransformation);

	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp,trafo);

			min->x = aisgl_min(min->x,tmp.x);
			min->y = aisgl_min(min->y,tmp.y);
			min->z = aisgl_min(min->z,tmp.z);

			max->x = aisgl_max(max->x,tmp.x);
			max->y = aisgl_max(max->y,tmp.y);
			max->z = aisgl_max(max->z,tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		GetBoundingBoxForNode(nd->mChildren[n],min,max,trafo);
	}
	*trafo = prev;
}

// ----------------------------------------------------------------------------
void OBJScene::GetBoundingBox (aiVector3D* min, aiVector3D* max)
{
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	min->x = min->y = min->z =  1e10f;
	max->x = max->y = max->z = -1e10f;
	GetBoundingBoxForNode(scene->mRootNode,min,max,&trafo);
}

void OBJScene::Color4ToFloat4(aiColor4D *c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

// ----------------------------------------------------------------------------
void OBJScene::SetFloat4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

void OBJScene::SetModelSizes()
{
	aiVector3D min, max;
	GetBoundingBox (&min, &max);
	_boundMax = Vector3f(max.x, max.y, max.z);
	_boundMin = Vector3f(min.x, min.y, min.z);
	float initialWidth = _boundMax.x() - _boundMin.x();
	float size = _sceneWidth/initialWidth;
	_modelScaleFactorX = size;
	_modelScaleFactorY = size;
	_modelScaleFactorZ = size;
}

OBJScene::OBJScene(QString name, Vector3f* position, AngleAxisf* rotation, float sceneWidth): VirtualObject(name, position, rotation)
{
	fileName = name;
	scene = NULL;
	scene_list = 0;
	_boundMax = Vector3f(0,0,0);
	_boundMin = Vector3f(0,0,0);
	_sceneWidth = sceneWidth;
	_modelScaleFactorX = 1;
	_modelScaleFactorY = 1;
	_modelScaleFactorZ = 1;

	_vertexbuffers = QList<GLuint>();
	_indexbuffers = QList<GLuint>();
	_indexSizes = QList<int>();
	_faceModes = QList<GLenum>();
	//QList<GLuint> _colorbuffers;
	_normalbuffers = QList<GLuint>();
	_uvbuffers = QList<GLuint>();
	_textureIds = QList<std::string>();

	_textureIdMap = QMap<std::string, GLuint>();
	_shading = WithMaterial;
}


OBJScene::~OBJScene(void)
{
}

void OBJScene::Load()
{
	LoadAsset(fileName);
}

bool OBJScene::LoadAsset(QString fileName)
{
	// we are taking one of the postprocessing presets to avoid
	// spelling out 20+ single postprocessing flags here.
	QString path = "Resources/Models/"+ fileName;
	QByteArray ba = path.toLocal8Bit();
	const char *c_str2 = ba.data();
	
	Assimp::Importer importer;
	scene = importer.ReadFile(c_str2, 0/*aiProcess_JoinIdenticalVertices | aiProcess_SortByPType*/);
	if( !scene) {
		fprintf( stderr, importer.GetErrorString());
		getchar();
		return false;
	}
	else {
		SetModelSizes();
		LoadGLTextures(scene);
		RecursiveLoading(scene, scene->mRootNode);
		return true;
	}
	qDebug() << "ERROR: Asset " << path << " not found.";
	return false;
}
int OBJScene::LoadGLTextures(const aiScene* scene)
{
	if (scene->HasTextures()) qDebug() << "Support for meshes with embedded textures is not implemented";
	
	for (unsigned int m=0; m<scene->mNumMaterials; m++)
	{
		int texIndex = 0;
		aiReturn texFound = AI_SUCCESS;

		aiString path;	// filename

		while (texFound == AI_SUCCESS)
		{
			texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
			if(path.length != 0)
			{
				GLuint texture = 0;
				LoadTexture(QString::fromStdString(path.data),&texture);
				_textureIdMap.insert(path.data, texture);
				texIndex++;
			}
		}
	}
	return true;

}
void OBJScene::RecursiveLoading (const aiScene *sc, aiNode* nd)
{
	int t;
	aiMatrix4x4 m = nd->mTransformation;
	// load all meshes assigned to this node
	for (int n = 0; n < nd->mNumMeshes; n++)
	{
		const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		bool hasNormals = true, hasColors = true;

		if(mesh->mNormals == NULL)
		{
			qDebug() << "WARNING: Mesh needs data of normals to ensure correct lightning!";
			hasNormals = false;
		}

		if(mesh->mColors[0] != NULL)
		{
			qDebug() << "WARNING: Mesh needs data of normals to ensure correct lightning!";
			hasColors = false;
		}

		std::vector<std::vector<double>> vertices;
		std::vector<std::vector<double>> tc;
		std::vector<std::vector<double>> normals;
		std::vector<std::vector<double>> faces;
		std::vector<std::vector<double>> ftc;
		std::vector<std::vector<double>> fn;
		igl::readOBJ("Resources/Models/"+fileName.toStdString(), vertices, tc,normals,faces,ftc,fn);

		GLenum face_mode;
		int correctNumIndices = 0;
		vector<unsigned short> indices;
		vector<glm::vec3> indexed_vertices;
		vector<glm::vec3> smoothed_vertices;
		vector<glm::vec2> indexed_uvs;
		vector<glm::vec3> indexed_normals;
		vector<vector<double>> vV;
		vector<vector<double>> vSV;
		vector<vector<int>> vF;
		GLuint vertexBuffer;
		GLuint normalBuffer;
		GLuint uvBuffer;
		GLuint indexBuffer;

		if( mesh->mNumFaces > 0)
		{
			const struct aiFace* firstface = &mesh->mFaces[0];
			correctNumIndices = firstface->mNumIndices;
			switch(correctNumIndices)
			{
				case 1: face_mode = GL_POINTS; break;
				case 2: face_mode = GL_LINES; break;
				case 3: face_mode = GL_TRIANGLES; break;
				default: face_mode = GL_POLYGON; break;
			}
		}

		indexed_vertices.reserve(vertices.size());
		for(int i = 0; i<vertices.size(); i++)
		{
			vector<double> vec = vertices[i];
			indexed_vertices.push_back(glm::vec3(vec[0],vec[1],vec[2]));
		}
				
		indexed_uvs.reserve(tc.size());
		for(int i = 0; i<tc.size(); i++)
		{
			vector<double> vec = tc[i];
			indexed_uvs.push_back(glm::vec2(vec[0],vec[1]));
		}

		indexed_normals.reserve(normals.size());
		for(int i = 0; i<normals.size(); i++)
		{
			vector<double> vec = normals[i];
			indexed_normals.push_back(glm::vec3(vec[0],vec[1],vec[2]));
		}

		//indices.reserve(normals.size());
		for(int i = 0; i<faces.size(); i++)
		{
			vector<double> vec = faces[i];
			indices.push_back(vec[0]);
			indices.push_back(vec[1]);
			indices.push_back(vec[2]);
		}

		// Fill vertices positions
		//indexed_vertices.reserve(mesh->mNumVertices);
		//vV.reserve(mesh->mNumVertices*3);
		//for(int i=0; i<mesh->mNumVertices; i++){
		//	aiVector3D pos = mesh->mVertices[i];
		//	indexed_vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
		//	vector<double> vertex(3);
		//	vertex[0] = pos.x;
		//	vertex[1] = pos.y;
		//	vertex[2] = pos.z;
		//	vV.push_back(vertex);
		//}

		// Fill vertices texture coordinates
		//indexed_uvs.reserve(mesh->mNumVertices);
		//for(int i=0; i<mesh->mNumVertices; i++){
		//	aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
		//	indexed_uvs.push_back(glm::vec2(UVW.x, UVW.y));
		//}

		// Fill vertices normals
		//indexed_normals.reserve(mesh->mNumVertices);
		//for(int i=0; i<mesh->mNumVertices; i++){
		//	aiVector3D n = mesh->mNormals[i];
		//	indexed_normals.push_back(glm::vec3(n.x, n.y, n.z));
		//}

		// Fill indeces per face
		//for (int t = 0; t < mesh->mNumFaces; t++) {
		//	GLenum current_face_mode;
		//	const aiFace* face = &mesh->mFaces[t];
		//	if(correctNumIndices != face->mNumIndices)
		//	{
		//		qDebug() << "ERROR: Can't get along with different face modes in one mesh!";
		//		continue;
		//	}
		//	std::vector<int > f;
		//	for(int j = 0; j < face->mNumIndices; j++)		// go through all vertices in face
		//	{
		//		int vertexIndex = face->mIndices[j];
		//		indices.push_back(vertexIndex);
		//		f.push_back(vertexIndex);
		//	}
		//	vF.push_back(f);
		//}
		//



// Load a mesh in OFF format
  igl::readOBJ("Resources/Models/"+fileName.toStdString(), V, F);

  // Compute Laplace-Beltrami operator: #V by #V
  igl::cotmatrix(V,F,L);

  // Alternative construction of same Laplacian
  SparseMatrix<double> G,K;
  // Gradient/Divergence
  igl::grad(V,F,G);
  // Diagonal per-triangle "mass matrix"
  VectorXd dblA;
  igl::doublearea(V,F,dblA);
  // Place areas along diagonal #dim times
  const auto & T = 1.*(dblA.replicate(3,1)*0.5).asDiagonal();
  // Laplacian K built as discrete divergence of gradient or equivalently
  // discrete Dirichelet energy Hessian
  K = -G.transpose() * T * G;
  cout<<"|K-L|: "<<(K-L).norm()<<endl;
    U = V;
  //const auto &key_down = [](igl::Viewer &viewer,unsigned char key,int mod)->bool
  //{
  //  switch(key)
  //  {
  //    case 'r':
  //    case 'R':
  //      U = V;
  //      break;
  //    case ' ':
  //    {
	for(int i = 0; i<10; i++)
	{
        // Recompute just mass matrix on each step
        SparseMatrix<double> M;
        igl::massmatrix(U,F,igl::MASSMATRIX_TYPE_BARYCENTRIC,M);
        // Solve (M-delta*L) U = M*U
        const auto & S = (M - 0.001*L);
        Eigen::SimplicialLLT<Eigen::SparseMatrix<double > > solver(S);
        assert(solver.info() == Eigen::Success);
		
        U = solver.solve(M*U);
		igl::matrix_to_list(U, vSV);
		U = U.eval();
        // Compute centroid and subtract (also important for numerics)
        //VectorXd dblA;
        igl::doublearea(U,F,dblA);
        double area = 0.5*dblA.sum();
        MatrixXd BC;
        igl::barycenter(U,F,BC);
        RowVector3d centroid(0,0,0);
        for(int i = 0;i<BC.rows();i++)
        {
          centroid += 0.5*dblA(i)/area*BC.row(i);
        }
        U.rowwise() -= centroid;
        // Normalize to unit surface area (important for numerics)
        U.array() /= sqrt(area);
	}
		double newlength = igl::bounding_box_diagonal(U);
		double length = igl::bounding_box_diagonal(V);
		igl::matrix_to_list(U, vSV);
		smoothed_vertices.reserve(vSV.size());
		for(int i = 0; i<vSV.size(); i++)
		{
			vector<double> pos(3);
			pos = vSV[i];
			smoothed_vertices.push_back(glm::vec3(pos[0], pos[1], pos[2]));
		}
		Eigen::RowVector3d min_point1 = V.colwise().minCoeff();
		Eigen::RowVector3d max_point1 = V.colwise().maxCoeff();
		Eigen::RowVector3d centroid1  = 0.5*(min_point1 + max_point1);

		Eigen::RowVector3d min_point2 = U.colwise().minCoeff();
		Eigen::RowVector3d max_point2 = U.colwise().maxCoeff();
		Eigen::RowVector3d centroid3  = 0.5*(min_point2 + max_point2);
		//_additionalTranslation = new Vector3f(-centroid3.x()*_modelScaleFactorX,-centroid3.y()*_modelScaleFactorY,-centroid3.z()*_modelScaleFactorZ);
		// Generate 1 buffer, put the resulting identifier in vertexbuffer
		glGenBuffers(1, &vertexBuffer);

		// The following commands will talk about our 'vertexbuffer' buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
 
		// Give our vertices to OpenGL.
		glBufferData(GL_ARRAY_BUFFER, smoothed_vertices.size() * sizeof(glm::vec3), &smoothed_vertices[0], GL_STATIC_DRAW);

		//make uv-buffer
		glGenBuffers(1, &uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

		//make normal-buffer
		glGenBuffers(1, &normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

		// Generate a buffer for the indices as well
		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

		_vertexbuffers.append(vertexBuffer);
		if(mesh->mNormals != NULL)
			_normalbuffers.append(normalBuffer);
		else
			_normalbuffers.append(-1);

		if(mesh->HasTextureCoords(0))
			_uvbuffers.append(uvBuffer);
		else
			_uvbuffers.append(-1);

		_indexbuffers.append(indexBuffer);
		_indexSizes.append(indices.size());

		_faceModes.append(face_mode);

		aiString texPath;	//contains filename of texture
		int texIndex = 0;
		const aiMaterial *mtl = sc->mMaterials[mesh->mMaterialIndex];
		mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath);
		_textureIds.append(texPath.data);
	}

	// draw all children
	for (int n = 0; n < nd->mNumChildren; n++)
	{
		RecursiveLoading(sc, nd->mChildren[n]);
	}
}


void OBJScene::DrawBuffer(int index, GLuint texture)
{
	// Bind our texture in Texture Unit 0
	if(texture != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffers[index]);
	glVertexAttribPointer(
	   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _uvbuffers[index]);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, _normalbuffers[index]);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexbuffers[index]);

	// Draw the triangles !
	glDrawElements(
			_faceModes[index],      // mode
			_indexSizes[index],    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

}



void OBJScene::Draw(bool useTracking)
{
	for(int i = 0; i < _vertexbuffers.count(); i++)
	{
		GLuint texture = -1;
		if(_textureIdMap.contains(_textureIds[i]))
			texture = _textureIdMap[_textureIds[i]];
		DrawBuffer(i, texture);
	}
}


void OBJScene::ApplyMaterial(aiMaterial *mtl)
{
	float c[4];

	GLenum fill_mode;
	int ret1, ret2;
	aiColor4D diffuse;
	aiColor4D specular;
	aiColor4D ambient;
	aiColor4D emission;
	float shininess, strength;
	int two_sided;
	int wireframe;
	unsigned int max;

	SetFloat4(c, 0.8f, 0.8f, 0.8f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		Color4ToFloat4(&diffuse, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

	SetFloat4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
		Color4ToFloat4(&specular, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

	SetFloat4(c, 0.2f, 0.2f, 0.2f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
		Color4ToFloat4(&ambient, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

	SetFloat4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
		Color4ToFloat4(&emission, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

	max = 1;
	ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
	if(ret1 == AI_SUCCESS) {
    	max = 1;
    	ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
		if(ret2 == AI_SUCCESS)
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
        else
        	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    }
	else {
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
		SetFloat4(c, 0.0f, 0.0f, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
	}

	max = 1;
	if(AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, &max))
		fill_mode = wireframe ? GL_LINE : GL_FILL;
	else
		fill_mode = GL_FILL;
	glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

	max = 1;
	if((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
		glDisable(GL_CULL_FACE);
	else 
		glEnable(GL_CULL_FACE);
}

