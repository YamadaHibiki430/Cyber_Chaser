xof 0303txt 0032
template AnimTicksPerSecond {
 <9e415a43-7ba6-4a73-8743-b73d47e88476>
 DWORD AnimTicksPerSecond;
}

template Frame {
 <3d82ab46-62da-11cf-ab39-0020af71e433>
 [...]
}

template Matrix4x4 {
 <f6f23f45-7686-11cf-8f52-0040333594a3>
 array FLOAT matrix[16];
}

template FrameTransformMatrix {
 <f6f23f41-7686-11cf-8f52-0040333594a3>
 Matrix4x4 frameMatrix;
}

template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template MeshNormals {
 <f6f23f43-7686-11cf-8f52-0040333594a3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template Coords2d {
 <f6f23f44-7686-11cf-8f52-0040333594a3>
 FLOAT u;
 FLOAT v;
}

template MeshTextureCoords {
 <f6f23f40-7686-11cf-8f52-0040333594a3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}

template TextureFilename {
 <a42790e1-7810-11cf-8f52-0040333594a3>
 STRING filename;
}

template VertexDuplicationIndices {
 <b8d65549-d7c9-4995-89cf-53a9a8b031e3>
 DWORD nIndices;
 DWORD nOriginalVertices;
 array DWORD indices[nIndices];
}

template Animation {
 <3d82ab4f-62da-11cf-ab39-0020af71e433>
 [...]
}

template AnimationSet {
 <3d82ab50-62da-11cf-ab39-0020af71e433>
 [Animation <3d82ab4f-62da-11cf-ab39-0020af71e433>]
}

template FloatKeys {
 <10dd46a9-775b-11cf-8f52-0040333594a3>
 DWORD nValues;
 array FLOAT values[nValues];
}

template TimedFloatKeys {
 <f406b180-7b3b-11cf-8f52-0040333594a3>
 DWORD time;
 FloatKeys tfkeys;
}

template AnimationKey {
 <10dd46a8-775b-11cf-8f52-0040333594a3>
 DWORD keyType;
 DWORD nKeys;
 array TimedFloatKeys keys[nKeys];
}


AnimTicksPerSecond {
 4800;
}

Frame pCube4 {
 

 FrameTransformMatrix {
  20.000000,0.000000,0.000000,0.000000,0.000000,-0.000002,-40.000000,0.000000,0.000000,0.500000,-0.000000,0.000000,34.234861,-0.500000,0.000000,1.000000;;
 }

 Mesh pCube4 {
  24;
  -0.500000;0.500000;-0.500000;,
  0.500000;0.500000;0.500000;,
  0.500000;0.500000;-0.500000;,
  -0.500000;0.500000;0.500000;,
  -0.500000;0.500000;0.500000;,
  0.500000;-0.500000;0.500000;,
  0.500000;0.500000;0.500000;,
  -0.500000;-0.500000;0.500000;,
  -0.500000;-0.500000;0.500000;,
  0.500000;-0.500000;-0.500000;,
  0.500000;-0.500000;0.500000;,
  -0.500000;-0.500000;-0.500000;,
  -0.500000;-0.500000;-0.500000;,
  0.500000;0.500000;-0.500000;,
  0.500000;-0.500000;-0.500000;,
  -0.500000;0.500000;-0.500000;,
  0.500000;0.500000;-0.500000;,
  0.500000;-0.500000;0.500000;,
  0.500000;-0.500000;-0.500000;,
  0.500000;0.500000;0.500000;,
  -0.500000;-0.500000;-0.500000;,
  -0.500000;0.500000;0.500000;,
  -0.500000;0.500000;-0.500000;,
  -0.500000;-0.500000;0.500000;;
  12;
  3;0,1,2;,
  3;1,0,3;,
  3;4,5,6;,
  3;5,4,7;,
  3;8,9,10;,
  3;9,8,11;,
  3;12,13,14;,
  3;13,12,15;,
  3;16,17,18;,
  3;17,16,19;,
  3;20,21,22;,
  3;21,20,23;;

  MeshNormals {
   24;
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;1.000000;0.000000;,
   0.000000;0.000000;1.000000;,
   0.000000;0.000000;1.000000;,
   0.000000;0.000000;1.000000;,
   0.000000;0.000000;1.000000;,
   0.000000;-1.000000;-0.000000;,
   0.000000;-1.000000;-0.000000;,
   0.000000;-1.000000;-0.000000;,
   0.000000;-1.000000;-0.000000;,
   0.000000;0.000000;-1.000000;,
   0.000000;0.000000;-1.000000;,
   0.000000;0.000000;-1.000000;,
   0.000000;0.000000;-1.000000;,
   1.000000;0.000000;0.000000;,
   1.000000;0.000000;0.000000;,
   1.000000;0.000000;0.000000;,
   1.000000;0.000000;0.000000;,
   -1.000000;0.000000;0.000000;,
   -1.000000;0.000000;0.000000;,
   -1.000000;0.000000;0.000000;,
   -1.000000;0.000000;0.000000;;
   12;
   3;0,1,2;,
   3;1,0,3;,
   3;4,5,6;,
   3;5,4,7;,
   3;8,9,10;,
   3;9,8,11;,
   3;12,13,14;,
   3;13,12,15;,
   3;16,17,18;,
   3;17,16,19;,
   3;20,21,22;,
   3;21,20,23;;
  }

  MeshTextureCoords {
   24;
   0.570835;0.285651;,
   0.813655;0.043969;,
   0.814223;0.285651;,
   0.571404;0.043969;,
   0.025293;0.975758;,
   0.501050;0.024243;,
   0.501050;0.975758;,
   0.025293;0.024243;,
   0.543540;0.769015;,
   0.818204;0.527333;,
   0.841519;0.769015;,
   0.566855;0.527333;,
   0.566855;0.527333;,
   0.814223;0.285651;,
   0.818204;0.527333;,
   0.570835;0.285651;,
   0.897533;0.096970;,
   0.910069;0.903030;,
   0.897569;0.903031;,
   0.910032;0.096970;,
   0.897533;0.096970;,
   0.910069;0.903030;,
   0.897569;0.903031;,
   0.910032;0.096970;;
  }

  MeshMaterialList {
   1;
   12;
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0;

   Material {
    1.000000;1.000000;1.000000;1.000000;;
    26.580494;
    0.050000;0.050000;0.050000;;
    0.000000;0.000000;0.000000;;

    TextureFilename {
     "GLS_map_model_texture_vo2.png";
    }
   }
  }

  VertexDuplicationIndices {
   24;
   8;
   0,
   1,
   2,
   3,
   3,
   5,
   1,
   7,
   7,
   9,
   5,
   11,
   11,
   2,
   9,
   0,
   2,
   5,
   9,
   1,
   11,
   3,
   0,
   7;
  }
 }
}

AnimationSet Anim1 {
 

 Animation {
  

  AnimationKey {
   0;
   2;
   0;4;0.707107,0.707107,0.000000,0.000000;;,
   16000;4;0.707107,0.707107,0.000000,0.000000;;;
  }

  AnimationKey {
   1;
   2;
   0;3;20.000000,40.000000,0.500000;;,
   16000;3;20.000000,40.000000,0.500000;;;
  }

  AnimationKey {
   2;
   2;
   0;3;34.234861,-0.500000,0.000000;;,
   16000;3;34.234861,-0.500000,0.000000;;;
  }
  { pCube4 }
 }
}