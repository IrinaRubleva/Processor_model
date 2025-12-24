#include "../include/glwidget.h"
#include <QMouseEvent>
#include <QImage>
#include <QDebug>

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {}

GLWidget::~GLWidget()
{
    makeCurrent();
    delete texture;
    doneCurrent();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    program.addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
        attribute vec3 position;
        attribute vec2 texcoord;
        attribute vec3 colorAttr;
        uniform mat4 mvp;
        varying vec2 vTex;
        varying vec3 vColor;
        void main() {
            vTex = texcoord;
            vColor = colorAttr;
            gl_Position = mvp * vec4(position, 1.0);
        }
    )");

    program.addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
        varying vec2 vTex;
        varying vec3 vColor;
        uniform sampler2D tex;
        uniform int useTexture;
        void main() {
            if (useTexture == 1)
                gl_FragColor = texture2D(tex, vTex);
            else
                gl_FragColor = vec4(vColor, 1.0);
        }
    )");

    program.link();

    QImage img("../cpu_texture.png");
    
    if (img.isNull()) {
        img = QImage(1, 1, QImage::Format_RGB32);
        img.fill(Qt::gray);
        qDebug() << "Texture file not found, using default texture";
    }

    texture = new QOpenGLTexture(img.mirrored());
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    proj.setToIdentity();
    proj.perspective(45.0f, float(w) / float(h), 0.1f, 100.0f);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program.bind();
    texture->bind();

    QMatrix4x4 model;
    model.translate(0, 0, distance);
    model.rotate(angleX, 1, 0, 0);
    model.rotate(angleY, 0, 1, 0);

    program.setUniformValue("mvp", proj * model);

    struct Vertex { float x,y,z,u,v,r,g,b; };
    float w = 1.5f, h = 1.5f, d = 0.1f;

    Vertex verts[] = {
        // front
        {-w/2,-h/2, d/2, 0,0, 1,1,1}, { w/2,-h/2, d/2,1,0, 1,1,1},
        { w/2, h/2, d/2,1,1, 1,1,1}, {-w/2, h/2, d/2,0,1, 1,1,1},

        // back
        {-w/2,-h/2,-d/2,0,0,0,0.6f,0},{ w/2,-h/2,-d/2,0,0,0,0.6f,0},
        { w/2, h/2,-d/2,0,0,0,0.6f,0},{-w/2, h/2,-d/2,0,0,0,0.6f,0},

        // left
        {-w/2,-h/2,-d/2,0,0,0,0.35f,0},{-w/2,-h/2, d/2,0,0,0,0.35f,0},
        {-w/2, h/2, d/2,0,0,0,0.35f,0},{-w/2, h/2,-d/2,0,0,0,0.35f,0},

        // right
        { w/2,-h/2,-d/2,0,0,0,0.35f,0},{ w/2, h/2,-d/2,0,0,0,0.35f,0},
        { w/2, h/2, d/2,0,0,0,0.35f,0},{ w/2,-h/2, d/2,0,0,0,0.35f,0},

        // top
        {-w/2, h/2,-d/2,0,0,0,0.35f,0},{-w/2, h/2, d/2,0,0,0,0.35f,0},
        { w/2, h/2, d/2,0,0,0,0.35f,0},{ w/2, h/2,-d/2,0,0,0,0.35f,0},

        // botom
        {-w/2,-h/2,-d/2,0,0,0,0.35f,0},{ w/2,-h/2,-d/2,0,0,0,0.35f,0},
        { w/2,-h/2, d/2,0,0,0,0.35f,0},{-w/2,-h/2, d/2,0,0,0,0.35f,0},
    };

    GLuint idxs[] = {
        0,1,2, 2,3,0,        //front
        4,5,6, 6,7,4,        // bakc
        8,9,10,10,11,8,      // left
        12,13,14,14,15,12,   // rigth
        16,17,18,18,19,16,   // top
        20,21,22,22,23,20    // bottom
    };

    int posLoc = program.attributeLocation("position");
    int texLoc = program.attributeLocation("texcoord");
    int colLoc = program.attributeLocation("colorAttr");

    program.setAttributeArray(posLoc, GL_FLOAT, &verts[0].x, 3, sizeof(Vertex));
    program.setAttributeArray(texLoc, GL_FLOAT, &verts[0].u, 2, sizeof(Vertex));
    program.setAttributeArray(colLoc, GL_FLOAT, &verts[0].r, 3, sizeof(Vertex));

    program.enableAttributeArray(posLoc);
    program.enableAttributeArray(texLoc);
    program.enableAttributeArray(colLoc);

    //front текстура
    program.setUniformValue("useTexture", 1);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, idxs);

    program.setUniformValue("useTexture", 0);
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, idxs + 6);

    program.disableAttributeArray(posLoc);
    program.disableAttributeArray(texLoc);
    program.disableAttributeArray(colLoc);

    texture->release();
    program.release();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint delta = event->pos() - lastPos;
    lastPos = event->pos();

    angleX += delta.y() * 0.5f;
    angleY += delta.x() * 0.5f;

    update();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    distance += event->angleDelta().y() * 0.001f; 
    if(distance > -1.0f) distance = -1.0f;      
    if(distance < -20.0f) distance = -20.0f;   
    update();
}
