#ifndef OFX_SVG
#define OFX_SVG

//#define USE_OFXFTGL

#include "ofMain.h"
#include "ofxDisplayList.h"
//#include "ofxFTGLFont.h"

#include "ofxSVGXml.h"
#include "ofxSVGTypes.h"
#include "ofxVectorMath.h"

//-------------------------------------------------

class ofxSVG{

	public:

        // Loading
        //----------------------------------
		void load(string svgPath);
		void loadLayer(string svgPath, string layer);       /*not implemented*/

		// Debug
		//----------------------------------
		void setVerbose(bool verbose);                       /*not implemented*/

        // Drawing to screen
        //----------------------------------
		void draw();
		void drawLayer(string layerName);
		void drawLayer(int i);

        // Save & Drawing to svg
        //----------------------------------
		void save(string svgPath);                          /*not implemented*/
		void addLayer(string layerName);                    /*not implemented*/

		void rect(float x, float y, float w, float h);      /*not implemented*/
		void ellipse(float x, float y, float rx, float ry); /*not implemented*/
		void circle(float x, float y, float r);             /*not implemented*/

		void beginPolygon();                                /*not implemented*/
		void endPolygon();                                  /*not implemented*/
		void beginPath();                                   /*not implemented*/
		void endPath();                                     /*not implemented*/

		void vertex(float x, float y);                      /*not implemented*/
		//void bezierVertex(float x, float y);                /*not implemented*/
		void bezierVertex(float x0, float y0, float x1, float y1);
		void bezierQuadraticVertex(float x0, float y0, float x1, float y1);
		void bezierVertex(float x0, float y0, float x1, float y1, float x2, float y2);
		void stroke(string colorHex, int weight);                       /*not implemented*/
		void fill(string colorHex);                         /*not implemented*/

		void noFill();                                      /*not implemented*/
		void noStroke();                                    /*not implemented*/

		void setOpacity(float percent);                     /*not implemented*/

		void translate(float tx, float ty);                 /*not implemented*/
		void rotate(float r);

		void pushMatrix();                                  /*not implemented*/
		void popMatrix();                                   /*not implemented*/

		void setLayerActive(string layerName);               /*not implemented*/

		string      getLayerActive(string layerName);        /*not implemented*/
		void saveToFile(string filename);	

		vector< ofxSVGLayer >   layers;
    private:

        // Parsing
        //----------------------------------
        void parseLayer();

        void parseRect();
        void parseEllipse();
        void parseCircle();
        void parseLine();
        void parsePolyline();                               /*not implemented*/
        void parsePolygon();
        void parseText();
        void parsePath();
		void parseImage();

        // Taken from Theo ofxSVGLoader
        //----------------------------------
        void pathToPoints(string pathStr, ofxSVGPath* obj);

        // Matrix parsing
        //----------------------------------
        ofPoint posFromMatrix(string matrix);
        float   rotFromMatrix(string matrix);

        // Fonts map
        //--------------------------------------------
        #ifdef USE_OFXFTGL
            map<string, ofxFTGLFont> fonts;
        #else
            map<string, ofTrueTypeFont> fonts;
        #endif

        // SVG Data/Infos
        //----------------------------------
        string                  svgVersion;

        int                     docWidth;
        int                     docHeight;

		int                     currentIteration;

        // XML Stuffs
        //----------------------------------
        ofxSVGXml               svgXml;
	
		// save stuffs
		int						currentSaveNode;
		ofxSVGXml				saveXml;
		map<string, string>		currentAttributes;
		vector<ofxMatrix3x3>	matrices; 
		string createAttribute(string element, ...);
		void matrixFromString(string smat, ofxMatrix3x3 mat);
		void stringFromMatrix(string* smat, ofxMatrix3x3 mat);
	
		//  create root
        //----------------------------------
		void createRootSvg();
	
        // Debug
        //----------------------------------
        bool                    bVerbose;
};

#endif

