//==============================================================================
/*
    Software License Agreement (BSD License)
    Copyright (c) 2019-2021, AMBF
    (https://github.com/WPI-AIM/ambf)

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

    * Neither the name of authors nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    \author    <amunawar@wpi.edu>
    \author    Adnan Munawar
*/
//==============================================================================

#include "matcap_model_plugin.h"
#include <yaml-cpp/yaml.h>

using namespace std;

int afModelMatCapPlugin::init(const afModelPtr a_afModelPtr, const afModelAttribsPtr a_modelAttribs)
{   
    m_modelPtr = a_afModelPtr;

    YAML::Node specificationDataNode;
    specificationDataNode = YAML::Load(a_modelAttribs->getSpecificationData().m_rawData);

    YAML::Node matCapTextureFilenameNode = specificationDataNode["textures"]["matcap"];

    if (matCapTextureFilenameNode.IsDefined()){

            afPath matcapFilepath = matCapTextureFilenameNode.as<string>();
            // matcapFilepath.resolvePath(visualAttribs.m_meshFilepath.parent_path());
            cerr << "INFO! TRYING TO LOAD: " << matcapFilepath.c_str() << endl;
            cTexture2dPtr matCapTexture = cTexture2d::create();
            if(matCapTexture->loadFromFile(matcapFilepath.c_str())){
                cerr << "INFO! SUCCESFULLY LOADED MATCAP TEXTURE: " << matcapFilepath.c_str() << endl;
            }
            else{
                cerr << "ERROR! FAILED TO LOAD MATCAP TEXTURE: " << matCapTextureFilenameNode.as<string>() << endl;
                return -1;
            }

            try{
                for (afRigidBodyPtr rb : m_modelPtr->getRigidBodies()){
                    // for (o : m_modelPtr->getO)
                    if (rb->getShaderProgram()){
                        for (int mi = 0 ; mi < rb->getVisualObject()->getNumMeshes(); mi++){
                            rb->getVisualObject()->getMesh(mi)->m_metallicTexture = matCapTexture;
                            rb->getVisualObject()->getMesh(mi)->m_metallicTexture->setTextureUnit(GL_TEXTURE3);
                        }
                        rb->getShaderProgram()->setUniformi("uMatcapMap", C_TU_METALLIC);
                    }
                    else{
                        cerr << "WARNING! IGNORING MATCAP PLUGIN AS NO SHADER DEFINED FOR OBJECT: " << rb->getName() << endl;
                    }
                }
            }
            catch(YAML::Exception& e){
                cerr << "ERROR! Exception " << e.what() << endl;
                return -1;
            }
    }

    return 1;
}


void afModelMatCapPlugin::graphicsUpdate()
{
}

void afModelMatCapPlugin::physicsUpdate(double dt)
{
}

void afModelMatCapPlugin::reset()
{
}

bool afModelMatCapPlugin::close()
{
    return 1;
}
