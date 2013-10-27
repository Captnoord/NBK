#include "BR5Model.h"
#include "TextureLoader.h"

using namespace rendering;
using namespace std;

namespace loaders
{
	CBR5Model::CBR5Model(): 
		fileName(""), 
		vertexCount(0), 
		framesCount(0), 
		currentFrame(0),
		startFrame(0),
		endFrame(0),
		texture(0),
		m_textureCoordinates(NULL),
		m_vertexCoordinates(NULL),
		m_vertexCoordinatesAnim(NULL),
		m_animSpeed(1.0f),
		m_animNextFrame(0.0f),
		//normalCoordinates(NULL),
		interpolate(false),
		update(true),
		copy(false),
		connected(false),
		actionsCount(0),
		vbo(NULL)
	{
	}

	CBR5Model::CBR5Model(const CBR5Model &br5Model)
	{
		//vbo = br5Model.vbo

		fileName = br5Model.fileName;
		memcpy(textureName,br5Model.textureName,256);
		vertexCount = br5Model.vertexCount;
		framesCount = br5Model.framesCount;
		currentFrame = br5Model.currentFrame;
		startFrame = br5Model.startFrame;
		endFrame = br5Model.endFrame;
		texture = br5Model.texture;
		boundingBox = br5Model.boundingBox;

		actionsCount = br5Model.actionsCount;
		for (int ac=0; ac<actionsCount; ac++)
		{
			actions[ac] = br5Model.actions[ac];
		}

		m_textureCoordinates = br5Model.m_textureCoordinates;
		//vertexCoordinates = br5Model.vertexCoordinates;
		
		m_vertexCoordinates = new float[vertexCount*3];
		memcpy(m_vertexCoordinates,br5Model.m_vertexCoordinates,sizeof(float)*vertexCount*3);

		m_vertexCoordinatesAnim = br5Model.m_vertexCoordinatesAnim;
		m_animNextFrame = br5Model.m_animNextFrame;
		m_animSpeed = br5Model.m_animSpeed;
		interpolate = br5Model.interpolate;
		update = br5Model.update;
		connected = br5Model.connected;

		vbo = new CVBO(framesCount==1?CVBO::BT_STATIC_DRAW:CVBO::BT_STREAM_DRAW,false);
		vbo->setVertexData(vertexCount,3,sizeof(GLfloat),m_vertexCoordinates,CVBO::DT_FLOAT);
		vbo->setTextureData(CVBO::TU_UNIT0,texture,vertexCount,2,sizeof(GLfloat),m_textureCoordinates,CVBO::DT_FLOAT);
		vbo->setEnumMode(CVBO::EM_TRIANGLES);

		copy = true;
	}

	CBR5Model::~CBR5Model()
	{
		delete vbo;
		delete [] m_vertexCoordinates;

		if (!copy)
		{
			delete [] m_textureCoordinates;		

			if (framesCount>1)
			{
				for (unsigned int i=0; i<framesCount; i++)
				{
					delete [] m_vertexCoordinatesAnim[i];
				}

				delete [] m_vertexCoordinatesAnim;
			}
		}
	}

	bool CBR5Model::loadFromFile(std::string fileName)
	{
		/* ***************** 1. load the data ***************** */

		FILE *br5 = fopen(fileName.c_str(),"rb");

		if (!br5)
		{
			return false;
		}

		size_t fileSize = getFileSize(br5);

		byte *data = new byte[fileSize];
		byte *tmp = data;

		// read the full model data
		size_t read = fread(data, fileSize, 1, br5);
		read = read * fileSize;

		if (read!=fileSize)
		{
			delete [] tmp;
			fprintf(stderr, "Can't read %s BR5 model\n", fileName.c_str());
			fclose(br5);
			return false;
		}

		// read the mark
		char mark[3];
		memcpy(mark,data,3);
		data+=3;

		if (mark[0]!='B' || mark[1]!='R' || mark[2]!='5')
		{
			delete [] tmp;
			// file is not a valid br5 model
			fclose(br5);
			return false;
		}

		// read texture name
		memcpy(textureName,data,256);
		data+=256;

		// read number of vertices
		vertexCount = *(unsigned int*)data;
		data+=4;

		m_textureCoordinates = new float[vertexCount*2];

		// read texture coordinates
		memcpy(m_textureCoordinates,data,sizeof(float)*vertexCount*2);
		data+=sizeof(float)*vertexCount*2;

		// read number of frames
		framesCount = *(unsigned int*)data;
		data+=4;

		if (framesCount<=1)
		{
			// non animated version

			// read vertices
			m_vertexCoordinates = new float[vertexCount*3];
			memcpy(m_vertexCoordinates,data,sizeof(float)*vertexCount*3);
			data+=sizeof(float)*vertexCount*3;
		}
		else
		{
			// read vertices
			/*vertexCoordinates = new float[vertexCount*3];
			memcpy(vertexCoordinates,data,sizeof(float)*vertexCount*3);
			data+=sizeof(float)*vertexCount*3;*/

			// read other anims
			m_vertexCoordinatesAnim = new float*[framesCount];

			for (unsigned int i=0; i<framesCount; i++)
			{
				m_vertexCoordinatesAnim[i] = new float[vertexCount*3];
				memcpy(m_vertexCoordinatesAnim[i],data,sizeof(float)*vertexCount*3);
				data+=sizeof(float)*vertexCount*3;
			}

			m_vertexCoordinates = new float[vertexCount*3];

			memcpy(m_vertexCoordinates,m_vertexCoordinatesAnim[0],sizeof(float)*vertexCount*3);
		}

		fclose(br5);

		data = tmp;
		delete [] data;

		startFrame = 0;
		endFrame = framesCount-1;


		/* ***************** 3. setup the VBO ***************** */

		vbo = new CVBO(framesCount==1?CVBO::BT_STATIC_DRAW:CVBO::BT_STREAM_DRAW,false);
		vbo->setVertexData(vertexCount,3,sizeof(GLfloat),m_vertexCoordinates,CVBO::DT_FLOAT);
		vbo->setEnumMode(CVBO::EM_TRIANGLES);

		setupTexture();

		// calculate the bounding box
		boundingBox.reset();
		for (unsigned int v=0; v<vertexCount*3; v+=3)
		{
			boundingBox.update(m_vertexCoordinates+v);
		}
		boundingBox.calculateExtents();

		return true;
	}

	bool CBR5Model::loadFromMD3(std::string fileName)
	{
	

		return true;
	}

	char *CBR5Model::getTextureName()
	{
		return textureName;
	}

	void CBR5Model::setTextureName(std::string textureName)
	{
		strcpy(this->textureName,textureName.c_str());

		setupTexture();
	}

	void CBR5Model::setupTexture()
	{
		if (textureName[0]!='0')
		{
			CTextureLoader::buildTexture(textureName,texture,true);
		}

		if (texture!=0)
		{
			vbo->setTextureData(CVBO::TU_UNIT0,texture,vertexCount,2,sizeof(GLfloat),m_textureCoordinates,CVBO::DT_FLOAT);
		}
	}

	long CBR5Model::getFileSize(FILE *file)
	{
		fseek(file,0,SEEK_END);
		long pos = ftell(file);
		fseek(file,0,SEEK_SET);
		return pos;
	}

	void CBR5Model::setAnimSpeed(float animSpeed)
	{
		this->m_animSpeed = animSpeed;
	}

	void CBR5Model::draw(GLfloat deltaTime)
	{
		if (framesCount>1)
		{
			if(update)
			{
				m_animNextFrame+=m_animSpeed*deltaTime;

				if (m_animNextFrame>=1.0f)
				{
					m_animNextFrame=1.0;
				}

				if (interpolate)
				{
					if (currentFrame<endFrame+(connected?1:0))
					{
						// interpolate between 2 frames
						for (unsigned int v=0; v<vertexCount*3; v+=3)
						{
							// newPos = nextFrame*animNextFrame+currentFrame*(1.0f-animNextFrame)

							GLint nextFrameIndex = currentFrame+1;

							if (connected && nextFrameIndex == endFrame)
							{
								nextFrameIndex = startFrame;
							}

							m_vertexCoordinates[v+0] = m_vertexCoordinatesAnim[nextFrameIndex][v+0]*m_animNextFrame+m_vertexCoordinatesAnim[currentFrame][v+0]*(1.0f-m_animNextFrame);
							m_vertexCoordinates[v+1] = m_vertexCoordinatesAnim[nextFrameIndex][v+1]*m_animNextFrame+m_vertexCoordinatesAnim[currentFrame][v+1]*(1.0f-m_animNextFrame);
							m_vertexCoordinates[v+2] = m_vertexCoordinatesAnim[nextFrameIndex][v+2]*m_animNextFrame+m_vertexCoordinatesAnim[currentFrame][v+2]*(1.0f-m_animNextFrame);
						}
					}
				}

				if (m_animNextFrame>=1.0f)
				{
					m_animNextFrame = 0.0f;

					currentFrame++;

					if (currentFrame>=endFrame)
					{
						currentFrame = startFrame;
					}

					if (!interpolate)
					{
						memcpy(m_vertexCoordinates,m_vertexCoordinatesAnim[currentFrame],sizeof(GLfloat)*vertexCount*3);
					}
				}		
			}
		}

		vbo->draw();
	}

	void CBR5Model::setInterpolate(bool interpolate)
	{
		this->interpolate = interpolate;
	}

	void CBR5Model::setConnected(bool connected)
	{
		this->connected = connected;
	}

	void CBR5Model::setUpdate(bool update)
	{
		this->update = update;
	}

	int CBR5Model::registerAction(int startFrame, int endFrame)
	{
		actions[actionsCount++] = pair<int, int>(startFrame,endFrame);
		return actionsCount-1;
	}

	void CBR5Model::doAction(int actionID)
	{
		m_animNextFrame = 0;

		pair<int,int> framePair = actions[actionID];

		currentFrame = startFrame = framePair.first;

		endFrame = framePair.second;
	}

	void CBR5Model::scaleToMaxY(float maxY)
	{
		float dx = maxY/boundingBox.height;

		boundingBox.reset();
		for (unsigned int v=0; v<vertexCount*3; v+=3)
		{
			m_vertexCoordinates[v+0]*=dx;
			m_vertexCoordinates[v+1]*=dx;
			m_vertexCoordinates[v+2]*=dx;
			boundingBox.update(m_vertexCoordinates+v);
		}
		boundingBox.calculateExtents();

		// update the anim part
		if (framesCount>1)
		{
			for (unsigned int i=0; i<framesCount; i++)
			{
				for (unsigned int v=0; v<vertexCount*3; v+=3)
				{
					m_vertexCoordinatesAnim[i][v+0]*=dx;
					m_vertexCoordinatesAnim[i][v+1]*=dx;
					m_vertexCoordinatesAnim[i][v+2]*=dx;
				}
			}
		}
		else
		{
			// we need to reupload the modified vertices
			vbo->setVertexData(vertexCount,3,sizeof(GLfloat),m_vertexCoordinates,CVBO::DT_FLOAT);
		}
	}

	sBoundingBox *CBR5Model::getBoundingBox()
	{
		return &boundingBox;
	}

	CVBO *CBR5Model::getVBO()
	{
		return vbo;
	}

	unsigned int CBR5Model::getVertexCount()
	{
		return vertexCount;
	}

	float *CBR5Model::getVertexCoordinates()
	{
		return m_vertexCoordinates;
	}

	float CBR5Model::getAnimSpeed()
	{
		return m_animSpeed;
	}
};