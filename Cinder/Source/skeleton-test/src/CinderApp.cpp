#include "cinder/MayaCamUI.h"
#include "cinder/TriMesh.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/Material.h"

#include <sstream>

#include "SkeletonData.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class CinderApp : public AppBasic {
public:
	void prepareSettings(Settings *settings);

	void setup();
	void update();
	void draw();

	//! loads the shaders
	void loadShaders();
	
protected:
	vector<Joint *> m_joints;

	SkeletonData * s;

	//! our Phong shader, which supports multiple targets
	gl::GlslProg	mPhongShader;
	
};

void CinderApp::prepareSettings(Settings *settings){
	settings->setWindowSize(900, 600);
	settings->setFrameRate(100.0f);
	settings->setTitle("Saving Bone Data");
}

void CinderApp::setup(){

	m_joints = vector<Joint *>();
	
	Joint * b1 = new Joint();
	b1->pos = Vec3f(-1.0666666666666667,0.00000000000000000,0.31333333333333324);
	Joint * b2 = new Joint();
	b2->pos = Vec3f(-1.0733333333333333,0.00000000000000000,-0.83999999999999997);
	Joint * b3 = new Joint();
	b3->pos = Vec3f(0.99333333333333318,0.00000000000000000,-0.82000000000000006);
	Joint * b4 = new Joint();
	b4->pos = Vec3f(0.99333333333333318,0.00000000000000000,0.82666666666666666);
	Joint * b5 = new Joint();
	b5->pos = Vec3f(-1.0800000000000001,0.00000000000000000,0.46666666666666656);

	m_joints.push_back(b1);
	m_joints.push_back(b2);
	m_joints.push_back(b3);
	m_joints.push_back(b4);
	m_joints.push_back(b5);

	console() << "setup" << endl;
	s = new SkeletonData();
	console() << "saveSkeleton" << endl;
	s->SaveSkeleton(m_joints);
}

void CinderApp::update(){}

void CinderApp::draw(){
	
	
}

void CinderApp::loadShaders()
{
	try{
		mPhongShader = gl::GlslProg( loadFile("../assets/shaders/phong.vert"), loadFile("../assets/shaders/phong.frag") );
	}catch( const std::exception &e ) {
		console() << e.what() << std::endl;
		quit();
	}
}


CINDER_APP_BASIC( CinderApp, RendererGl )