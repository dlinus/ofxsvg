#include "ofxSVG.h"
#include <fstream>

//--------------------------------------------------------------
void ofxSVG::load(string svgPath){
    bVerbose = true;

    if(bVerbose){
        cout<<"ofxSVG: Loading \""<<svgPath<<"\""<<endl<<endl;
    }

    TiXmlBase::SetCondenseWhiteSpace(false);

    ifstream file(ofToDataPath(svgPath).c_str());

    // Check if File Exist
    //--------------------------------------------
    if (!file && bVerbose){
        printf("ofxSVG: File not found.");
        return;
    }
    else {
        // Extract string from file
        //----------------------------------------
        string line;
        string svgString;
        while (getline(file, line)) {
            svgString += "\n"+line;
        }

        // Check if SVG
        //----------------------------------------
        if(svgString.find("<svg") == -1 && bVerbose){
            printf("ofxSVG: Unknown Format.");
            return;
        }
        else {
            // Remove Header
            //------------------------------------
            int svgTagPos = svgString.find("<svg");
            svgString = svgString.substr(svgTagPos, svgString.length() - svgTagPos);

            // Load File to TinyXml
            //--------------------------------------------
            svgXml.loadFromBuffer(svgString);
        }

        file.close();
    }

    // Get Metadatas
    //--------------------------------------------
    svgVersion  = svgXml.getAttribute("svg", "version", "undefined");

    docWidth    = svgXml.getAttribute("svg", "width", 0);
    docHeight   = svgXml.getAttribute("svg", "height", 0);

    if(bVerbose){
        cout<<"ofxSVG: SVG Version: "<<svgVersion<<endl;
        cout<<"ofxSVG: Document size: "<<docWidth<<"x"<<docHeight<<endl;
    }

    // Enter SVG Main Layer
    //--------------------------------------------
    svgXml.pushTag("svg");

        int pops;
        if(svgXml.tagExists("switch")) {
            svgXml.pushTag("switch");
            pops++;
        }
        if(svgXml.tagExists("g")) {
            svgXml.pushTag("g");
            pops++;
        }

            // Read Number of Layers
            //----------------------------------------
            int nLayers = svgXml.getNumTags("g");

            // Only one layer
            //----------------------------------------
            if(nLayers == 0){
                if(bVerbose){
                    cout<<"ofxSVG: Loading one layers."<<endl;
                }

                ofxSVGLayer layer;
                layer.name = svgXml.getAttribute("g", "id", "");
                layers.push_back( layer );

                parseLayer();
            }
            // Multiple Layers
            //----------------------------------------
            else {
                if(bVerbose){
                    cout<<"ofxSVG: Loading "<<nLayers<<" layers."<<endl;
                }
                for(int i=0; i<nLayers; i++){
                    ofxSVGLayer layer;
                    layer.name = svgXml.getAttribute("g", "id", "", i);
                    layers.push_back( layer );

                    if(bVerbose){
                        cout<<"ofxSVG:    Layer \""<<layer.name<<"\" :"<<endl;
                    }

                    svgXml.pushTag("g", i);
                        parseLayer();
                    svgXml.popTag();
                }
            }

        for(int i=0;i<pops; i++) svgXml.popTag();

    svgXml.popTag();

    if(bVerbose){
        cout<<endl<<"ofxSVG: Loading succes!"<<endl;
    }

    TiXmlBase::SetCondenseWhiteSpace(true);

}

// Parsing
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void ofxSVG::parseLayer(){
    int layerId = (int) (layers.size()) - 1;

    // Get Number of Objects in this Layer
    //--------------------------------------------
    int numObjects = svgXml.getNumTags();
    if(bVerbose) cout<<"ofxSVG:        numObjects: "<<numObjects<<endl;


    // Loop threw all objects, parse and render them
    //--------------------------------------------
    for(int i=0; i<numObjects; i++) {
        currentIteration = i;

        string name = svgXml.getName(i);

        if(name == "rect")  parseRect();
        else if(name == "circle") parseCircle();
        else if(name == "ellipse") parseEllipse();
        else if(name == "line") parseLine();
        else if(name == "polygon") parsePolygon();
        else if(name == "text") parseText();
        else if(name == "path")parsePath();
        else if(name == "g"){
            svgXml.pushTag(i);
                parseLayer();
            svgXml.popTag();
        }
    }

}

//-------------------------------------------------------------------------------------

void ofxSVG::parseRect(){
    float x         = ofToFloat(svgXml.getAttribute("x", currentIteration));
    float y         = ofToFloat(svgXml.getAttribute("y", currentIteration));
    float width     = ofToFloat(svgXml.getAttribute("width", currentIteration));
    float height    = ofToFloat(svgXml.getAttribute("height", currentIteration));
    string id       = svgXml.getAttribute("id", currentIteration);

    // Extract Rotation from
    //------------------------------------
    string transform = svgXml.getAttribute("transform", currentIteration);

    string fill = svgXml.getAttribute("fill", currentIteration);
    string stroke = svgXml.getAttribute("stroke", currentIteration);
    string opacity = svgXml.getAttribute("opacity", currentIteration);
    float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;

    if(!(fill=="none" && stroke=="")){

        ofxSVGRectangle* obj = new ofxSVGRectangle;

        // Shape info
        //--------------------------------
        obj->type        = ofxSVGObject_Rectangle;
        obj->renderMode  = ofxSVGRender_DisplayList;
        obj->name        = id;
        obj->x           = x;
        obj->y           = y;
        obj->width       = width;
        obj->height      = height;

        // Display List
        //--------------------------------
        obj->dl.begin();

        glPushMatrix();

        if(transform!=""){
            float rot = rotFromMatrix(transform);
            glTranslatef(-width/2,-height/2,0);
            glRotatef(rot, 0, 0, 1);
        }

        if(fill!="none"){
            ofFill();
            if(fill!="") {
                int rgb = strtol(("0x"+fill.substr(1, fill.length()-1)).c_str(), NULL, 0);
                float r = (rgb >> 16) & 0xFF;
                float g = (rgb >> 8) & 0xFF;
                float b = (rgb) & 0xFF;
                ofSetColor(r,g,b,alpha);
            }
            else ofSetColor(0,0,0,alpha);
            ofRect(x, y,width,height);
        }

        if(stroke!="" && stroke!="none"){
            string strokeWeight = svgXml.getAttribute("stroke-width", currentIteration);
            if(strokeWeight!="") ofSetLineWidth(ofToInt(strokeWeight));
            ofNoFill();
            int rgb = strtol(("0x"+stroke.substr(1, stroke.length()-1)).c_str(), NULL, 0);
            float r = (rgb >> 16) & 0xFF;
            float g = (rgb >> 8) & 0xFF;
            float b = (rgb) & 0xFF;
            ofSetColor(r,g,b,alpha);
            ofRect(x,y,width,height);
            if(strokeWeight!="") ofSetLineWidth(1);
        }

        glPopMatrix();

        obj->dl.end();

        // Vertexs
        //--------------------------------
        obj->vertexs.push_back(ofPoint(x, y));
        obj->vertexs.push_back(ofPoint(x+width, y));
        obj->vertexs.push_back(ofPoint(x+width, y+height));
        obj->vertexs.push_back(ofPoint(x, y+height));

        layers[layers.size()-1].objects.push_back(obj);
    }
}

//-------------------------------------------------------------------------------------

void ofxSVG::parseCircle(){
    float x     = ofToFloat(svgXml.getAttribute("cx", currentIteration));
    float y     = ofToFloat(svgXml.getAttribute("cy", currentIteration));
    float r     = ofToFloat(svgXml.getAttribute("r", currentIteration));

    string id       = svgXml.getAttribute("id", currentIteration);

    string fill = svgXml.getAttribute("fill", currentIteration);
    string stroke = svgXml.getAttribute("stroke", currentIteration);
    string opacity = svgXml.getAttribute("opacity", currentIteration);
    float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;



    ofxSVGCircle* obj = new ofxSVGCircle;

    // Shape info
    //--------------------------------

    obj->type        = ofxSVGObject_Circle;
    obj->renderMode  = ofxSVGRender_DisplayList;
    obj->name        = id;
    obj->x           = x;
    obj->x           = y;
    obj->r           = r;

    // Display List
    //--------------------------------
    obj->dl.begin();

    if(fill!="none"){
        ofFill();
        if(fill!="") {
            int rgb = strtol(("0x"+fill.substr(1, fill.length()-1)).c_str(), NULL, 0);
            float r = (rgb >> 16) & 0xFF;
            float g = (rgb >> 8) & 0xFF;
            float b = (rgb) & 0xFF;
            ofSetColor(r,g,b,alpha);
        }
        else ofSetColor(0,0,0,alpha);
        ofCircle(x,y,r);
    }

    if(stroke!="" && stroke!="none"){
        string strokeWeight = svgXml.getAttribute("stroke-width", currentIteration);
        if(strokeWeight!="") ofSetLineWidth(ofToInt(strokeWeight));
        ofNoFill();
        int rgb = strtol(("0x"+stroke.substr(1, stroke.length()-1)).c_str(), NULL, 0);
        if(rgb!=0){
            float r = (rgb >> 16) & 0xFF;
            float g = (rgb >> 8) & 0xFF;
            float b = (rgb) & 0xFF;
            ofSetColor(r,g,b,alpha);
        }
        else ofSetColor(0,0,0,alpha);
        ofSetLineWidth(1);
        ofCircle(x,y,r);
        if(strokeWeight!="") ofSetLineWidth(1);
    }

    obj->dl.end();

    // Vertexs
    //--------------------------------

    int res = 30;
    float angle = 0.0f;
    float theta = M_TWO_PI / (float) res;
	for(int i = 0; i < res; i++){
		obj->vertexs.push_back(ofPoint(x + cos(angle) * r, y + sin(angle) * r));
		angle+=theta;
	}

    layers[layers.size()-1].objects.push_back(obj);
}

//-------------------------------------------------------------------------------------

void ofxSVG::parseEllipse(){
    float x     = ofToFloat(svgXml.getAttribute("cx", currentIteration));
    float y     = ofToFloat(svgXml.getAttribute("cy", currentIteration));
    float rx     = ofToFloat(svgXml.getAttribute("rx", currentIteration))*2.0;
    float ry     = ofToFloat(svgXml.getAttribute("ry", currentIteration))*2.0;

    string id       = svgXml.getAttribute("id", currentIteration);

    string fill = svgXml.getAttribute("fill", currentIteration);
    string stroke = svgXml.getAttribute("stroke", currentIteration);
    string opacity = svgXml.getAttribute("opacity", currentIteration);
    float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;


    ofxSVGEllipse* obj = new ofxSVGEllipse;

    // Shape info
    //--------------------------------

    obj->type        = ofxSVGObject_Ellipse;
    obj->renderMode  = ofxSVGRender_DisplayList;
    obj->name        = id;
    obj->x           = x;
    obj->y           = x;
    obj->rx          = rx;
    obj->ry          = ry;

    // Display List
    //--------------------------------
    obj->dl.begin();

    if(fill!="none"){
        ofFill();
        if(fill!="") {
            int rgb = strtol(("0x"+fill.substr(1, fill.length()-1)).c_str(), NULL, 0);
            float r = (rgb >> 16) & 0xFF;
            float g = (rgb >> 8) & 0xFF;
            float b = (rgb) & 0xFF;
            ofSetColor(r,g,b,alpha);
        }
        else ofSetColor(0,0,0,alpha);
        ofEllipse(x,y,rx,ry);
    }

    if(stroke!="" && stroke!="none"){
        string strokeWeight = svgXml.getAttribute("stroke-width", currentIteration);
        if(strokeWeight!="") ofSetLineWidth(ofToInt(strokeWeight));
        ofNoFill();
        int rgb = strtol(("0x"+stroke.substr(1, stroke.length()-1)).c_str(), NULL, 0);
        float r = (rgb >> 16) & 0xFF;
        float g = (rgb >> 8) & 0xFF;
        float b = (rgb) & 0xFF;
        ofSetColor(r,g,b,alpha);
        ofEllipse(x,y,rx,ry);
        if(strokeWeight!="") ofSetLineWidth(1);
    }

    obj->dl.end();

    // Vertexs
    //--------------------------------

    int res = 30;
    float angle = 0.0f;
    float theta = M_TWO_PI / (float) res;
	for(int i = 0; i < res; i++){
		obj->vertexs.push_back(ofPoint(x + cos(angle) * rx * 0.5f, y + sin(angle) * ry * 0.5f));
		angle+=theta;
	}

    layers[layers.size()-1].objects.push_back(obj);
}

//-------------------------------------------------------------------------------------

void ofxSVG::parseLine(){
    float x1     = ofToFloat(svgXml.getAttribute("x1", currentIteration));
    float y1     = ofToFloat(svgXml.getAttribute("y1", currentIteration));
    float x2     = ofToFloat(svgXml.getAttribute("x2", currentIteration));
    float y2     = ofToFloat(svgXml.getAttribute("y2", currentIteration));

    string id       = svgXml.getAttribute("id", currentIteration);

    string fill = svgXml.getAttribute("fill", currentIteration);
    string stroke = svgXml.getAttribute("stroke", currentIteration);
    string opacity = svgXml.getAttribute("opacity", currentIteration);
    float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;

    ofxSVGLine* obj = new ofxSVGLine;

    // Shape info
    //--------------------------------

    obj->type        = ofxSVGObject_Line;
    obj->renderMode  = ofxSVGRender_DisplayList;
    obj->name        = id;
    obj->x1          = x1;
    obj->y1          = y1;
    obj->x2          = x2;
    obj->y2          = y2;

    // Display List
    //--------------------------------
    obj->dl.begin();

    if(stroke!="" && stroke!="none"){
        string strokeWeight = svgXml.getAttribute("stroke-width", currentIteration);
        if(strokeWeight!="") ofSetLineWidth(ofToInt(strokeWeight));
        ofNoFill();
        int rgb = strtol(("0x"+stroke.substr(1, stroke.length()-1)).c_str(), NULL, 0);
        float r = (rgb >> 16) & 0xFF;
        float g = (rgb >> 8) & 0xFF;
        float b = (rgb) & 0xFF;
        ofSetColor(r,g,b,alpha);
        ofLine(x1,y1,x2,y2);
        if(strokeWeight!="") ofSetLineWidth(1);
    }

    obj->dl.end();

    // Vertexs
    //--------------------------------
    obj->vertexs.push_back(ofPoint(x1, y1));
    obj->vertexs.push_back(ofPoint(x2, y2));

    layers[layers.size()-1].objects.push_back(obj);
}

//-------------------------------------------------------------------------------------

void ofxSVG::parsePolygon(){
    string id       = svgXml.getAttribute("id", currentIteration);
    string fill = svgXml.getAttribute("fill", currentIteration);
    string stroke = svgXml.getAttribute("stroke", currentIteration);
    string opacity = svgXml.getAttribute("opacity", currentIteration);
    float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;

    vector<string> pointsStrings = ofSplitString(svgXml.getAttribute("points", currentIteration), " ");

    ofxSVGPolygon* obj = new ofxSVGPolygon;

    // Shape info
    //--------------------------------

    obj->type        = ofxSVGObject_Polygon;
    obj->renderMode  = ofxSVGRender_DisplayList;
    obj->name        = id;

    // Vertexs
    //--------------------------------
    for(int i=0; i<pointsStrings.size()-1; i++){
        vector<string> pointString = ofSplitString(pointsStrings[i], ",");
        obj->vertexs.push_back(ofPoint(strtod(pointString[0].c_str(), NULL), strtod(pointString[1].c_str(), NULL)));
    }

    // Display List
    //--------------------------------
    obj->dl.begin();

    if(fill!="none"){
        ofFill();
        if(fill!="") {
            int rgb = strtol(("0x"+fill.substr(1, fill.length()-1)).c_str(), NULL, 0);
            float r = (rgb >> 16) & 0xFF;
            float g = (rgb >> 8) & 0xFF;
            float b = (rgb) & 0xFF;
            ofSetColor(r,g,b,alpha);
        }
        else ofSetColor(0,0,0,alpha);

        ofBeginShape();
        for(int i=0; i<obj->vertexs.size(); i++) ofVertex(obj->vertexs[i].x, obj->vertexs[i].y);
        ofEndShape(OF_CLOSE);
    }

    if(stroke!="" && stroke!="none"){
        string strokeWeight = svgXml.getAttribute("stroke-width", currentIteration);
        if(strokeWeight!="") ofSetLineWidth(ofToInt(strokeWeight));
        ofNoFill();
        int rgb = strtol(("0x"+stroke.substr(1, stroke.length()-1)).c_str(), NULL, 0);
        float r = (rgb >> 16) & 0xFF;
        float g = (rgb >> 8) & 0xFF;
        float b = (rgb) & 0xFF;
        ofSetColor(r,g,b,alpha);

        ofBeginShape();
        for(int i=0; i<obj->vertexs.size(); i++) ofVertex(obj->vertexs[i].x, obj->vertexs[i].y);
        ofEndShape(OF_CLOSE);

        if(strokeWeight!="") ofSetLineWidth(1);
    }

    obj->dl.end();

    layers[layers.size()-1].objects.push_back(obj);
}

void ofxSVG::parseText(){

    svgXml.pushTag(currentIteration);
    int numTSpans = svgXml.getNumTags("tspan");
    svgXml.popTag();

    // Multiline Text
    //------------------------------------
    if(numTSpans>0){

        // Extract XY From matrix
        //------------------------------------
        ofPoint pos = posFromMatrix(svgXml.getAttribute("transform", currentIteration));

        // Get Alpha
        //------------------------------------
        string opacity = svgXml.getAttribute("opacity", currentIteration);
        float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;

        svgXml.pushTag(currentIteration);

        // Parse and Load Fonts
        //--------------------------------
        for(int j=0; j<numTSpans; j++){
            int fontSize    = svgXml.getAttribute("tspan", "font-size", 0, j);
            string fontName = svgXml.getAttribute("tspan", "font-family", "", j);
            fontName        = fontName.substr(1, fontName.length() - 2);
            string fontExt  = ".ttf";


            // Check if Font is already loaded
            //------------------------------------
            if(fonts.count(fontName+ofToString(fontSize)) == 0){
                #ifdef USE_OFXFTGL
                    ofxFTGLFont* font = new ofxFTGLFont();
                #else
                    ofTrueTypeFont* font = new ofTrueTypeFont();
                #endif

                // Find Font Extension
                // and check if file exist
                //--------------------------------
                ifstream ttfFile(ofToDataPath("fonts/"+fontName+fontExt).c_str());
                if(!ttfFile) fontExt = ".otf";

                ifstream otfFile(ofToDataPath("fonts/"+fontName+fontExt).c_str());
                if(!otfFile && bVerbose)
                    printf("ofxSVG: Font file '%s' not found or format not supported\n", fontName.c_str());
                else otfFile.close();

                // Load font and add to font map
                //--------------------------------
                #ifdef USE_OFXFTGL
                    font->loadFont("fonts/"+fontName+fontExt, fontSize);
                #else
                    font->loadFont("fonts/"+fontName+fontExt, fontSize*0.75f, true, true, true);
                #endif

                fonts.insert(make_pair(fontName+ofToString(fontSize), font));
            }
        }

        ofxSVGText* obj = new ofxSVGText;

        // Shape info
        //--------------------------------
        obj->type        = ofxSVGObject_Text;
        obj->renderMode  = ofxSVGRender_DisplayList;
        obj->name        = svgXml.getAttribute("id", currentIteration);

        // Display List
        //--------------------------------
        obj->dl.begin();
        for(int j=0; j<numTSpans; j++){

            // Parse Current Text Metadata
            //------------------------------------
            float x         = svgXml.getAttribute("tspan", "x", 0.0f, j) + pos.x;
            float y         = svgXml.getAttribute("tspan", "y", 0.0f, j) + pos.y;

            string text     = svgXml.getValue("tspan", "", j);

            int fontSize    = svgXml.getAttribute("tspan", "font-size", 0, j);
            string fontName = svgXml.getAttribute("tspan", "font-family", "", j);
            fontName        = fontName.substr(1, fontName.length() - 2);

            obj->positions.push_back(ofPoint(x, y));
            obj->texts.push_back(text);
            obj->fonts.push_back(fontName+ofToString(fontSize));

            // Draw font
            //------------------------------------
            if(svgXml.attributeExists("tspan", "fill", j)){
                string col = svgXml.getAttribute("tspan", "fill", "", j);
                int color = strtol(("0x"+col.substr(1, col.length()-1)).c_str(), NULL, 0);
                float r = (color >> 16) & 0xFF;
                float g = (color >> 8) & 0xFF;
                float b = (color) & 0xFF;
                ofSetColor(r,g,b,alpha);
                obj->colors.push_back(color);
            }
            else {
                ofSetColor(0,0,0,alpha);
                obj->colors.push_back(0);
            }

            fonts[fontName+ofToString(fontSize)]->drawString(text, x, y);
        }
        obj->dl.end();

        layers[layers.size()-1].objects.push_back(obj);

        svgXml.popTag();
    }
    // Single Line Text
    //------------------------------------
    else {

        // Extract XY From matrix
        //------------------------------------
        ofPoint pos = posFromMatrix(svgXml.getAttribute("transform", currentIteration));

        // Get Alpha
        //------------------------------------
        string opacity = svgXml.getAttribute("opacity", currentIteration);
        float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;

        // Parse Current Text Metadata
        //------------------------------------
        string text     = svgXml.getValue(currentIteration);

        int fontSize    = strtol(svgXml.getAttribute("font-size", currentIteration).c_str(), NULL, 0);
        string fontName = svgXml.getAttribute("font-family", currentIteration);
        fontName        = fontName.substr(1, fontName.length() - 2);
        string col = svgXml.getAttribute("fill", currentIteration);
        string fontExt  = ".ttf";

        // Check if Font is already loaded
        //------------------------------------
        if(fonts.count(fontName+ofToString(fontSize)) == 0){
            #ifdef USE_OFXFTGL
                ofxFTGLFont* font = new ofxFTGLFont();
            #else
                ofTrueTypeFont* font = new ofTrueTypeFont();
            #endif

            // Find Font Extension
            // and check if file exist
            //--------------------------------
            ifstream ttfFile(ofToDataPath("fonts/"+fontName+fontExt).c_str());
            if(!ttfFile) fontExt = ".otf";

            ifstream otfFile(ofToDataPath("fonts/"+fontName+fontExt).c_str());
            if(!otfFile && bVerbose)
                printf("ofxSVG: Font file '%s' not found or format not supported\n", fontName.c_str());
            else otfFile.close();

            // Load font and add to font map
            //--------------------------------
            #ifdef USE_OFXFTGL
                font->loadFont("fonts/"+fontName+fontExt, fontSize);
            #else
                font->loadFont("fonts/"+fontName+fontExt, fontSize*0.75f, true, true, true);
            #endif

            fonts.insert(make_pair(fontName+ofToString(fontSize), font));
        }

        ofxSVGText* obj = new ofxSVGText;

        // Shape info
        //--------------------------------

        obj->type        = ofxSVGObject_Text;
        obj->renderMode  = ofxSVGRender_DisplayList;
        obj->name        = svgXml.getAttribute("id", currentIteration);

        // Display List
        //--------------------------------

        obj->dl.begin();

        // Draw font
        //------------------------------------
        if(col!="" && col!="none"){
            int color = strtol(("0x"+col.substr(1, col.length()-1)).c_str(), NULL, 0);
            float r = (color >> 16) & 0xFF;
            float g = (color >> 8) & 0xFF;
            float b = (color) & 0xFF;
            ofSetColor(r,g,b,alpha);
        }
        else ofSetColor(0,0,0,alpha);

        fonts[fontName+ofToString(fontSize)]->drawString(text, pos.x, pos.y);

        obj->dl.end();

        layers[layers.size()-1].objects.push_back(obj);
    }

}

//-------------------------------------------------------------------------------------

void ofxSVG::parsePath(){
    string pathStr = svgXml.getAttribute("d", currentIteration);
    string sepPath = "";

    for (int j = 0; j < pathStr.size(); j++){
        char c = pathStr[j];

        if ( c < 31 )continue;

        if ( c == 'M' || c == 'm' ||
                        c == 'L' || c == 'l' ||
                        c == 'V' || c == 'v' ||
                        c == 'H' || c == 'h' ||
                        c == 'C' || c == 'c' ||
                        c == 'S' || c == 's' ||
                        c == 'z' || c == 'Z' ){
            sepPath += "*";
            sepPath.push_back(c);
            //sepPath += "*";
        }
        else if (c == '-' || c == ' ' || c == ','){
            if ( c == '-'){
                if ( sepPath.length() > 0 && pathStr[currentIteration-1] >= '0' && pathStr[j-1] <= '9' ){
                    sepPath.push_back('|');
                }
                sepPath.push_back(c);
            }
            else {
                sepPath.push_back('|');
            }
        }
        else {
            sepPath.push_back(c);
        }

    }


    string fill = svgXml.getAttribute("fill", currentIteration);
    string stroke = svgXml.getAttribute("stroke", currentIteration);
    string opacity = svgXml.getAttribute("opacity", currentIteration);
    float alpha = (opacity=="") ? 255.0f : ofToFloat(opacity) * 255.0f;

    ofxSVGPath* obj = new ofxSVGPath;

    // Shape info
    //--------------------------------

    obj->type        = ofxSVGObject_Path;
    obj->renderMode  = ofxSVGRender_DisplayList;
    obj->name        = svgXml.getAttribute("id", currentIteration);

    // Display List
    //--------------------------------

    obj->dl.begin();

    if(fill!="none"){
        ofFill();
        if(fill!="") {
            int rgb = strtol(("0x"+fill.substr(1, fill.length()-1)).c_str(), NULL, 0);
            float r = (rgb >> 16) & 0xFF;
            float g = (rgb >> 8) & 0xFF;
            float b = (rgb) & 0xFF;
            ofSetColor(r,g,b,alpha);
        }
        else ofSetColor(0,0,0,alpha);

        pathToPoints(sepPath);
    }

    if(stroke!="" && stroke!="none"){
        string strokeWeight = svgXml.getAttribute("stroke-width", currentIteration);
        if(strokeWeight!="") ofSetLineWidth(ofToInt(strokeWeight));
        ofNoFill();
        int rgb = strtol(("0x"+stroke.substr(1, stroke.length()-1)).c_str(), NULL, 0);
        float r = (rgb >> 16) & 0xFF;
        float g = (rgb >> 8) & 0xFF;
        float b = (rgb) & 0xFF;
        ofSetColor(r,g,b,alpha);

        pathToPoints(sepPath);

        if(strokeWeight!="") ofSetLineWidth(1);
    }


    obj->dl.end();

    layers[layers.size()-1].objects.push_back(obj);
}

//-------------------------------------------------------------------------------------




ofPoint ofxSVG::posFromMatrix(string matrix){
    matrix = matrix.substr(7, matrix.length()-8);
    vector<string> matrixStrings = ofSplitString(matrix, " ");
    return ofPoint(ofToFloat(matrixStrings[4]), ofToFloat(matrixStrings[5]));
}


float ofxSVG::rotFromMatrix(string matrix){
    int i = matrix.find("(");
    if(i!=-1){
        matrix = matrix.substr(i+1, matrix.length()-(i+2));
        vector<string> matrixStrings = ofSplitString(matrix, " ");
        return ofRadToDeg(atan2(ofToFloat(matrixStrings[3]),ofToFloat(matrixStrings[0])));
    }
    return 0.0f;
}

// Taken from Theo ofxSVGLoader
//--------------------------------------------------------------------
void ofxSVG::pathToPoints(string pathStr){
    ofPoint drawPt;

    //ofxSvgShape currShape;

    ofBeginShape();

    vector <string> commandStr = ofSplitString(pathStr, "*");
    bool lastPointIsBezier = false;
    int points = 0;
    ofPoint lastBezierC2;

    for (int i = 0; i < commandStr.size(); i++)
    {
        if ( commandStr[i].length() ==  0 )continue;

        string currStr = commandStr[i];
        char command = currStr[0];

        currStr = currStr.substr(1, currStr.length()-1);

        //printf("[%c], curString is %s\n", command, currStr.c_str());

        vector <string> coordsStr = ofSplitString(currStr, "|");
        vector <float> coords;

        for (int i = 0; i < coordsStr.size(); i++)
        {
            coords.push_back(ofToFloat(coordsStr[i]));
        }

        int numP = coords.size();

        if ( ( command == 'm' || command == 'M' ) && numP >= 2)
        {
            if ( command == 'm')
            {
                drawPt.x += coords[0];
                drawPt.y += coords[1];
            }
            else
            {
                drawPt.x = coords[0];
                drawPt.y = coords[1];
            }
            ofVertex(drawPt.x, drawPt.y);
            lastPointIsBezier = false;
            points++;
        }

        if ( ( command == 'v' || command == 'V' ) && numP >= 1 )
        {

            for (int j = 0; j < numP; j++)
            {

                if ( command == 'v')
                {
                    drawPt.y += coords[j+0];
                }
                else
                {
                    drawPt.y = coords[j+0];
                }
                ofVertex(drawPt.x, drawPt.y);
                lastPointIsBezier = false;
                points++;
            }
        }

        if ( ( command == 'h' || command == 'H' ) && numP >= 1 )
        {

            for (int j = 0; j < numP; j++)
            {

                if ( command == 'h')
                {
                    drawPt.x += coords[j+0];
                }
                else
                {
                    drawPt.x = coords[j+0];
                }
                ofVertex(drawPt.x, drawPt.y);
                lastPointIsBezier = false;
                points++;
            }
        }

        if ( ( command == 'l' || command == 'L' )  && numP >= 2 )
        {

            for (int j = 0; j < numP; j+=2)
            {

                if ( command == 'l')
                {
                    drawPt.x += coords[j+0];
                    drawPt.y += coords[j+1];
                }
                else
                {
                    drawPt.x = coords[j+0];
                    drawPt.y = coords[j+1];
                }
                ofVertex(drawPt.x, drawPt.y);
                lastPointIsBezier = false;
                points++;
            }
        }

        if ( ( command == 'c' || command == 'C' )  && numP >= 6 )
        {

            for (int j = 0; j < numP; j+=6)
            {

                float c1x, c1y;
                float c2x, c2y;

                if ( command == 'c')
                {
                    c1x = drawPt.x + coords[j+0];
                    c1y = drawPt.y + coords[j+1];
                    c2x = drawPt.x + coords[j+2];
                    c2y = drawPt.y + coords[j+3];
                    drawPt.x += coords[j+4];
                    drawPt.y += coords[j+5];
                }
                else
                {
                    c1x = coords[j+0];
                    c1y = coords[j+1];
                    c2x = coords[j+2];
                    c2y = coords[j+3];
                    drawPt.x = coords[j+4];
                    drawPt.y = coords[j+5];
                }
                ofBezierVertex(c1x, c1y, c2x, c2y, drawPt.x, drawPt.y);
                lastBezierC2 = ofPoint(c2x, c2y);
                lastPointIsBezier = true;
                points++;
            }
        }

        if ( ( command == 's' || command == 'S' )  && numP >= 4 )
        {

            for (int j = 0; j < numP; j+=4)
            {

                ofPoint prePoint = drawPt;

                ofPoint preHandle;
                if (points && lastPointIsBezier)
                {
                    preHandle = lastBezierC2 - prePoint;
                }
                else
                {
                    preHandle = drawPt - prePoint;
                }

                float c1x, c1y;
                float c2x, c2y;

                if ( command == 's')
                {
                    c1x = prePoint.x - preHandle.x;
                    c1y = prePoint.y - preHandle.y;

                    c2x = drawPt.x + coords[j+0];
                    c2y = drawPt.y + coords[j+1];
                    drawPt.x += coords[j+2];
                    drawPt.y += coords[j+3];
                }
                else
                {
                    c1x = prePoint.x - preHandle.x;
                    c1y = prePoint.y - preHandle.y;

                    c2x = coords[j+0];
                    c2y = coords[j+1];
                    drawPt.x = coords[j+2];
                    drawPt.y = coords[j+3];
                }
                ofBezierVertex(c1x, c1y, c2x, c2y, drawPt.x, drawPt.y);
                lastPointIsBezier = true;
                lastBezierC2 = ofPoint(c2x, c2y);
                points++;
            }
        }

    }

    ofEndShape();
}

//--------------------------------------------------------------
void ofxSVG::draw(){
    for(int i=0; i<layers.size(); i++){
        layers[i].draw();

    }
}
void ofxSVG::drawLayer(string layerName){
    for(int i=0; i<layers.size(); i++){
        if(layers[i].name == layerName){
            layers[i].draw();
            return;
        }
    }
}

void ofxSVG::drawLayer(int i){
    layers[i].draw();
}
//-------------------------------------------------------------------------