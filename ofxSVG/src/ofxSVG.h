#ifndef OFX_SVG
#define OFX_SVG

//#define USE_OFXFTGL

#include "ofMain.h"
#include "ofxDisplayList.h"
//#include "ofxFTGLFont.h"
#include "ofXSVGPathParser.h"

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
		void save(string svgPath);                          
		void addLayer(string layerName);                    
		void rect(float x, float y, float w, float h);      
		void ellipse(float x, float y, float rx, float ry);
		void circle(float x, float y, float r);

		void beginPolygon();                   
		void endPolygon();                     
		void beginPath();                      
		void endPath();                        

		void vertex(float x, float y);
		//void bezierVertex(float x, float y);                
		void bezierVertex(float x0, float y0, float x1, float y1);
		void bezierQuadraticVertex(float x0, float y0, float x1, float y1);
		void bezierVertex(float x0, float y0, float x1, float y1, float x2, float y2);
		void stroke(string colorHex, int weight);                       
		void fill(string colorHex);                         

		void noFill();                                      
		void noStroke();                                    

		void setOpacity(float percent);                     

		void translate(float tx, float ty);
		void rotate(float r);

		void pushMatrix();                                  /*not implemented*/
		void popMatrix();                                   /*not implemented*/
		
		void setLayerActive(string layerName);               /*not implemented*/

		string      getLayerActive(string layerName);        /*not implemented*/
		void saveToFile(string filename);	
		ofxVec2f scaleFromMatrix(string matrix);
	float scale(string scaleVal);
	
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
		void parsePathExperimental();
		void parseImage();

        // Taken from Theo ofxSVGLoader
        //----------------------------------
        void pathToVectorData(string pathStr, ofxSVGPath* obj);
		void vectorDataToVertexs(ofxSVGPath* path, float resampleDist);
		vector<ofPoint> singleBezierToPtsWithResample(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float resampleDist);
		void drawVectorData(ofxSVGPath* path);
	
		void drawVectorDataExperimental(ofxComplexSVGPath* object);


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

