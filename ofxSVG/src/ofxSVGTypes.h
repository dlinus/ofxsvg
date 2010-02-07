#ifndef OFX_SVGTYPES
#define OFX_SVGTYPES

enum {
    ofxSVGObject_Text,
    ofxSVGObject_Rectangle,
    ofxSVGObject_Circle,
    ofxSVGObject_Ellipse,
    ofxSVGObject_Line,
    ofxSVGObject_Polygon,
    ofxSVGObject_Path,

    ofxSVGRender_DirectMode,
    ofxSVGRender_DisplayList,
    ofxSVGRender_VertexArray,
    ofxSVGRender_VertexBufferObject,
};

// GENERIC OBJECT
//-------------------------------------------------

class ofxSVGObject {
    public:
    void draw(){
        switch(renderMode){
            case ofxSVGRender_DirectMode:
            break;
            case ofxSVGRender_DisplayList:
                dl.draw();
            break;
            case ofxSVGRender_VertexArray:
            break;
            case ofxSVGRender_VertexBufferObject:
            break;
            default:
                printf("OfxSVGObject: RenderMode Undefined");
            break;
        }
    }
    int             renderMode;
    int             type;

    string          name;

    int             fill;
    int             stroke;
    int             strokeWeight;
    float           opacity;

    ofxDisplayList  dl;

    vector<ofPoint> vertexs;
};

class ofxSVGRectangle : public ofxSVGObject {
public:
    float x, y, width, height;
};

class ofxSVGEllipse : public ofxSVGObject {
public:
    float x, y, rx, ry;
};

class ofxSVGCircle : public ofxSVGObject {
public:
    float x, y, r;
};

class ofxSVGLine : public ofxSVGObject {
public:
    float x1, y1, x2, y2;
};

class ofxSVGText : public ofxSVGObject {
public:
    vector<ofPoint> positions;
    vector<string>  texts;
    vector<string>  fonts;
    vector<int>  colors;
};

class ofxSVGPolygon : public ofxSVGObject {
public:
};

class ofxSVGPath : public ofxSVGObject {
public:
};


// LAYER
//-------------------------------------------------

class ofxSVGLayer {
    public:

    void draw(){
        for(int i=0; i<objects.size(); i++){
            objects[i]->draw();
        }
    }

    string                  name;
    vector<ofxSVGObject*>    objects;
};


#endif
