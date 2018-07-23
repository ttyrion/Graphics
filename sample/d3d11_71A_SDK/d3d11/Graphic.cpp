#include "stdafx.h"
#include "Graphic.h"
#include "utility.h"


Graphic::Graphic(GraphicEngine* engine) {
    engine_ = engine;
}

Graphic::~Graphic() {

}

void Graphic::Release() {
    engine_ = NULL;
    ReleaseCOMInterface(vertex_buffer_);
    ReleaseCOMInterface(index_buffer_);
}
