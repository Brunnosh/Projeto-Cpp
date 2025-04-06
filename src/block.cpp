#include "block.h"

//declarar blocos
/*





float block[] = {
    //TEXTURE COORDS -> BOTTOM LEFT VERTEX = MIN UV \ TOP RIGHT VERTEX -> MAX UV

    // Face frontal
    -0.5f, -0.5f,  0.5f,  uvGrassSide.uMin, uvGrassSide.vMin,
     0.5f, -0.5f,  0.5f,  uvGrassSide.uMax, uvGrassSide.vMin,
     0.5f,  0.5f,  0.5f,  uvGrassSide.uMax, uvGrassSide.vMax,
    -0.5f,  0.5f,  0.5f,  uvGrassSide.uMin, uvGrassSide.vMax,


    // Face traseira
    -0.5f, -0.5f, -0.5f,  uvGrassSide.uMax, uvGrassSide.vMin,
     0.5f, -0.5f, -0.5f,  uvGrassSide.uMin, uvGrassSide.vMin,
     0.5f,  0.5f, -0.5f,  uvGrassSide.uMin, uvGrassSide.vMax,
    -0.5f,  0.5f, -0.5f,  uvGrassSide.uMax, uvGrassSide.vMax,



    // Face esquerda
    -0.5f, -0.5f, -0.5f,  uvGrassSide.uMin, uvGrassSide.vMin,
    -0.5f, -0.5f,  0.5f,  uvGrassSide.uMax, uvGrassSide.vMin,
    -0.5f,  0.5f,  0.5f,  uvGrassSide.uMax, uvGrassSide.vMax,
    -0.5f,  0.5f, -0.5f,  uvGrassSide.uMin, uvGrassSide.vMax,

    // Face direita
     0.5f, -0.5f, -0.5f,  uvGrassSide.uMax, uvGrassSide.vMin,
     0.5f, -0.5f,  0.5f,  uvGrassSide.uMin, uvGrassSide.vMin,
     0.5f,  0.5f,  0.5f,  uvGrassSide.uMin, uvGrassSide.vMax,
     0.5f,  0.5f, -0.5f,  uvGrassSide.uMax, uvGrassSide.vMax,

     // Face inferior
     -0.5f, -0.5f, -0.5f,  uvDirtTop.uMin, uvDirtTop.vMin,
      0.5f, -0.5f, -0.5f,  uvDirtTop.uMax, uvDirtTop.vMin,
      0.5f, -0.5f,  0.5f,  uvDirtTop.uMax, uvDirtTop.vMax,
     -0.5f, -0.5f,  0.5f,  uvDirtTop.uMin, uvDirtTop.vMax,

     // Face superior
     -0.5f,  0.5f, -0.5f,  uvGrassTop.uMin, uvGrassTop.vMin,
      0.5f,  0.5f, -0.5f,  uvGrassTop.uMax, uvGrassTop.vMin,
      0.5f,  0.5f,  0.5f,  uvGrassTop.uMax, uvGrassTop.vMax,
     -0.5f,  0.5f,  0.5f,  uvGrassTop.uMin, uvGrassTop.vMax
};


      //indices para desenhar as faces CCW

        unsigned int indices[] = {
        // Face frontal (olhando para +Z)
        0, 1, 2,
        2, 3, 0,

        // Face traseira (olhando para -Z) - invertido
        5,4,7,
        7,6,5,

        // Face esquerda (olhando para -X) - invertido
        8, 9, 10,
        10, 11, 8, 

        // Face direita (olhando para +X)
        13, 12, 15,
        15, 14, 13,

        // Face inferior (olhando para -Y)
        16,17,18,
        18,19,16,

        // Face superior (olhando para +Y) - invertido
        23,22,21,
        21,20,23
    };


























*/