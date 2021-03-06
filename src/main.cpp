#include "ofAppGLFWWindow.h"
#include "ofMain.h"
#include "ofxGui.h"
#include "fluid.h"

class Fire : public ofBaseApp{

	public:
		ofxPanel gui;
		ofxToggle dvel;
        ofxToggle dden;
		ofxToggle addX;
        ofxToggle addY;
        ofxToggle addZ;
		ofxToggle TaddSource;
		ofxToggle drawAbstacle, drawAxis;

		ofxButton Bclear;
		ofShader shader;

        ofFbo fbo;

		ofxSlider<float> dt;
		ofxSlider<float> diff;
		ofxSlider<float> visc;
		ofxSlider<float> force;
		ofxSlider<float> source;
		ofxSlider<float> source_alpha;
		ofxSlider<float> size_cube;

		ofxSlider<float> sourcePosX;
		ofxSlider<float> sourcePosY;
		ofxSlider<float> sourcePosZ;
		ofImage img;

		ofEasyCam camera;
		int dw,dh;

        int allocate_data ()  {
            int size     = (M+2)*(N+2)*(O+2);
            u            = (float *) malloc ( size*sizeof(float) );
            v            = (float *) malloc ( size*sizeof(float) );
            w            = (float *) malloc ( size*sizeof(float) );
            u_prev       = (float *) malloc ( size*sizeof(float) );
            v_prev       = (float *) malloc ( size*sizeof(float) );
            w_prev       = (float *) malloc ( size*sizeof(float) );
            dens         = (float *) malloc ( size*sizeof(float) );
            dens_prev    = (float *) malloc ( size*sizeof(float) );
            if ( !u || !v || !w || !u_prev || !v_prev || !w_prev || !dens || !dens_prev ) {
                    fprintf ( stderr, "cannot allocate data\n" );
                    exit();
            }
            return  1 ;
        }

        void draw_velocity ( void ) {
            int i, j, k;
            float x, y, z, h;
            h = size_cube/MAX(MAX(M, N), MAX(N, O));
            glColor3f ( 1.0f, 1.0f, 1.0f );
            glLineWidth ( 1.0f );
            glBegin ( GL_LINES );
            for ( i=1; i<=M; i++ ) {
                    x = (i-0.5f)*h;
                    for ( j=1; j<=N; j++ ) {
                            y = (j-0.5f)*h;
                            for ( k=1; k<=O; k++ ) {
                                    z = (k-0.5f)*h;
                                    glVertex3f ( x, y, z );
                                    glVertex3f ( x+u[IX(i,j,k)], y+v[IX(i,j,k)], z+w[IX(i,j,k)] );
                            }
                    }
            }
            glEnd ();
        }

        void draw_density ( void ) {
            int i, j, k;
            float x, y,z, h, d000, d010, d100, d110,d001, d011, d101, d111;
            h = size_cube/MAX(MAX(M, N), MAX(N, O));
            glBegin ( GL_QUADS );
            for ( i=0; i<=M; i++ ) {
                    x = (i-0.5f)*h;
                    for ( j=0; j<=N; j++ ) {
                            y = (j-0.5f)*h;
                            for ( k=0; k<=O; k++ ) {
                                    z = (k-0.5f)*h;
                                    d000 = dens[IX(i,j,k)];
                                    d010 = dens[IX(i,j+1,k)];
                                    d100 = dens[IX(i+1,j,k)];
                                    d110 = dens[IX(i+1,j+1,k)];
                                    d001 = dens[IX(i,j,k+1)];
                                    d011 = dens[IX(i,j+1,k+1)];
                                    d101 = dens[IX(i+1,j,k+1)];
                                    d111 = dens[IX(i+1,j+1,k+1)];

                                    glColor4f ( d111, d111, d111, source_alpha ); glVertex3f ( x+h,y+h,z+h );
                                    glColor4f ( d011, d011, d011, source_alpha ); glVertex3f ( x, y+h, z+h);
                                    glColor4f ( d001, d001, d001, source_alpha ); glVertex3f ( x, y, z+h );
                                    glColor4f ( d101, d101, d101, source_alpha ); glVertex3f ( x+h, y, z+h );

                                    glColor4f ( d110, d110, d110, source_alpha ); glVertex3f ( x+h, y+h, z );
                                    glColor4f ( d111, d111, d111, source_alpha ); glVertex3f ( x+h,y+h,z+h );
                                    glColor4f ( d101, d101, d101, source_alpha ); glVertex3f ( x+h, y, z+h );
                                    glColor4f ( d100, d100, d100, source_alpha ); glVertex3f ( x+h, y, z );

                                    glColor4f ( d010, d010, d010, source_alpha ); glVertex3f ( x, y+h, z );
                                    glColor4f ( d110, d110, d110, source_alpha ); glVertex3f ( x+h, y+h, z );
                                    glColor4f ( d100, d100, d100, source_alpha ); glVertex3f ( x+h, y, z );
                                    glColor4f ( d000, d000, d000, source_alpha ); glVertex3f ( x, y, z );

                                    glColor4f ( d011, d011, d011, source_alpha ); glVertex3f ( x, y+h, z+h);
                                    glColor4f ( d010, d010, d010, source_alpha ); glVertex3f ( x, y+h, z );
                                    glColor4f ( d000, d000, d000, source_alpha ); glVertex3f ( x, y, z );
                                    glColor4f ( d001, d001, d001, source_alpha ); glVertex3f ( x, y, z+h );

                                    glColor4f ( d100, d100, d100, source_alpha ); glVertex3f ( x+h, y, z );
                                    glColor4f ( d000, d000, d000, source_alpha ); glVertex3f ( x, y, z );
                                    glColor4f ( d001, d001, d001, source_alpha ); glVertex3f ( x, y, z+h );
                                    glColor4f ( d101, d101, d101, source_alpha ); glVertex3f ( x+h, y, z+h );

                                    glColor4f ( d110, d110, d110, source_alpha ); glVertex3f ( x+h, y+h, z );
                                    glColor4f ( d010, d010, d010, source_alpha ); glVertex3f ( x, y+h, z );
                                    glColor4f ( d011, d011, d011, source_alpha ); glVertex3f ( x, y+h, z+h);
                                    glColor4f ( d111, d111, d111, source_alpha ); glVertex3f ( x+h, y+h, z+h );
                            }
                    }
            }
            glEnd ();
        }

        void get_force_source ( float * d, float * u, float * v, float * w ) {
            int i, j, k, size=(M+2)*(N+2)*(O+2);;
            for ( i=0 ; i<size ; i++ ) {
                    u[i] = v[i] = w[i]= d[i] = 0.0f;
            }
            if(addforce[0]==1) // x
            {
                    i=2  ,
                    j=N/2;
                    k=O/2;
                    if ( i<1 || i>M || j<1 || j>N || k <1 || k>O) return;
                    u[IX(i,j,k)] = force*10;
                    addforce[0] = 0;
            }
            if(addforce[1]==1)
            {
                    i=M/2 ,
                    j=2   ;
                    k=O/2 ;
                    if ( i<1 || i>M || j<1 || j>N || k <1 || k>O) return;
                    v[IX(i,j,k)] = force*10;
                    addforce[1] = 0;
            }
            if(addforce[2]==1) // y
            {
                    i=M/2,
                    j=N/2;
                    k=2;
                    if ( i<1 || i>M || j<1 || j>N || k <1 || k>O) return;
                    w[IX(i,j,k)] = force*10;
                    addforce[2] = 0;
            }
            if(addsource==1)
            {
                    i=M/2+sourcePosX,
                    j=N/2+sourcePosY;
                    k=O/2+sourcePosZ;
                    d[IX(i,j,k)] = source;
                    addsource = 0;
            }
            return;
        }

        void external_collision( ofQuaternion q ){

        }

        void update_fluid() {
            get_force_source( dens_prev, u_prev, v_prev, w_prev );
            vel_step ( M,N,O, u, v, w, u_prev, v_prev,w_prev, visc, dt );
            dens_step ( M,N,O, dens, dens_prev, u, v, w, diff, dt );
        }

        void clear_data ( ) {
            int i, size=(M+2)*(N+2)*(O+2);
            for ( i=0 ; i<size ; i++ ) {
                    u[i] = v[i] = w[i] = u_prev[i] = v_prev[i] =w_prev[i] = dens[i] = dens_prev[i] = 0.0f;
            }
            addforce[0] = addforce[1] = addforce[2] = 0;
        }

		void exit(){
            clear_data();
			cout<<"exit and clear!"<<endl;
		}

		void setup() {
            ofSetFrameRate(60);
            ofSetVerticalSync(true);

            dw=ofGetScreenWidth();
			dh=ofGetScreenHeight();
			ofDisableArbTex();
            fbo.allocate(dw,dh);
            shader.load("shaders/fluid.vert","shaders/fluid.frag");

			camera.setFarClip(100000);
			camera.setNearClip(.1);
            if ( !allocate_data () ){
                    cout<<"error allocate!"<<endl;
                    exit();
			}
            clear_data ();

			gui.setup();
            gui.setPosition(ofPoint(10,10));

			gui.add(size_cube.setup("size box", 100.0f,0.0f,255.0f));
            gui.add(dvel.setup("draw velocity", true));
            gui.add(dden.setup("draw density",  false));

			gui.add(TaddSource.setup("add source",false));
			gui.add(Bclear.setup("clear source",false));

			gui.add(addX.setup("add x",false));
			gui.add(addY.setup("add y",false));
			gui.add(addZ.setup("add z",false));
			gui.add(dt.setup("time delta", 0.9f,0.0f,25.0f));
			gui.add(diff.setup("diffuse",  0.0f,0.0f,25.0f));
			gui.add(visc.setup("viscosity", 0.0f,0.0f,25.0f));
			gui.add(force.setup("add force",30.0f,0.0f,60.0f));
			gui.add(source.setup("density", 200.0f,0.0f,600.0f));
			gui.add(source_alpha.setup("alpha",0.05,0.0,1.0));
			gui.add(drawAbstacle.setup("draw abstacle",false));
			gui.add(drawAxis.setup("draw Axis",true));

			gui.add(sourcePosX.setup("source posX", 0,-10,SIZE));
			gui.add(sourcePosY.setup("source posY", 0,-10,SIZE));
			gui.add(sourcePosZ.setup("source posZ", 0,-10,SIZE));
		}

		void update() {
			update_fluid();
			if(TaddSource) addsource = 1;
			if(addX) addforce[0] = 1;
            if(addY) addforce[1] = 1;
            if(addZ) addforce[2] = 1;
            if(Bclear)  clear_data();
		}

		void draw() {
			ofBackground(25);

			fbo.begin();
			ofClear(0,0,0,0);
			ofEnableDepthTest();
			camera.begin();
			    glPushMatrix();

			    glEnable(GL_BLEND);
		        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		        glEnable(GL_ALPHA_TEST);
		        glAlphaFunc(GL_GREATER, 0);

		        if(dvel) draw_velocity();
		        if(dden){
                        draw_density();
                }

                glDisable(GL_BLEND);
		        glDisable(GL_ALPHA_TEST);

		        glPopMatrix();
                if( drawAxis ){	ofDrawAxis(15); }

			camera.end();
			ofDisableDepthTest();
			fbo.end();

            shader.begin();
            shader.setUniformTexture("tex",fbo.getTextureReference(),0);
            shader.setUniform2f("resolution",dw,dh);
            shader.setUniform1f("timer",ofGetElapsedTimef());

            fbo.draw(0,0);

            shader.end();
			gui.draw();
		}

		void keyPressed(int key) {
			 switch (key) {
				case 'f' :
					ofToggleFullscreen();
					break;
             }
		}
        /* end */
};

int main(){
       ofAppGLFWWindow window;
       ofSetupOpenGL(&window, 1024,768, OF_WINDOW);
       ofRunApp( new Fire());
}
