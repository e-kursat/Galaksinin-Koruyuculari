#ifndef QOPENGLPANEL_H
#define QOPENGLPANEL_H

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtOpenGL/QOpenGLVertexArrayObject>
#include <QtOpenGL/QOpenGLBuffer>
#include <QFile>
#include <QMatrix4x4>
#include <QtMath>

#include <QImage>
#include <QOpenGLTexture>

#include <QFileInfo>

class QOpenGLPanel : public QOpenGLWidget
{
public:
    QOpenGLPanel(QWidget *parent = nullptr);
    ~QOpenGLPanel();
    void setCameraMatrix();
    void setProjectionMatrix();
    void translate(GLfloat x, GLfloat y, GLfloat z);
    void rotate(GLfloat degree, GLfloat x, GLfloat y, GLfloat z);
    void scale(GLfloat x, GLfloat y, GLfloat z);
    void lookAt(GLfloat ex, GLfloat ey, GLfloat ez, GLfloat cx, GLfloat cy, GLfloat cz, GLfloat ux, GLfloat uy, GLfloat uz);
    void perspective(GLfloat angle, GLfloat ratio, GLfloat near, GLfloat far);
    void resetScene();
    void mousePressEvent(QMouseEvent* event) override;

    GLuint createSphere(GLuint X_SEGMENTS, GLuint Y_SEGMENT, GLfloat x_offset, GLfloat y_offset, GLfloat z_offset);
    unsigned char* getObjectTextureData();
    GLuint loadTexture(QString fileName);

private:

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    const char* readShaderSource(QString filename);
    QOpenGLFunctions* getGLFunctions();
    QOpenGLExtraFunctions* getGLExtraFunctions();
    bool initializeShaderProgram(QString vertex, QString fragment, QOpenGLFunctions *f);
    bool checkGLError(QOpenGLFunctions *f, QString functionCall);

    GLuint progID, vertID, fragID;
    GLuint arrays, triangleData;
    GLuint position, color, normal, texture;

    GLuint translateMatrixID, rotateMatrixID, scaleMatrixID;
    QMatrix4x4 translateMatrix, rotateMatrix, scaleMatrix;
    GLfloat tX, tY, tZ;
    float rDegree;
    GLfloat rX, rY, rZ;
    GLfloat sX, sY, sZ;
    GLfloat reX, reY, reZ;


    GLuint projectionMatrixID, cameraMatrixID;
    QMatrix4x4 projectionMatrix, cameraMatrix;
    GLfloat camEyeX, camEyeY, camEyeZ;
    QVector3D cameraEye;
    GLfloat camCenterX, camCenterY, camCenterZ;
    QVector3D cameraCenter;
    GLfloat camUpX, camUpY, camUpZ;
    QVector3D cameraUp;
    GLfloat verticalAngle, aspectRatio, nearPlane, farPlane;

    std::vector<float> vert;
    GLuint sphereVAO[2], vbo[2], ebo[2];
    GLuint indexCount;

    QList<QVector3D> positions;
    QList<QVector2D> uv;
    QList<QVector3D> normals;

    std::vector<unsigned int> indices;

    QMatrix4x4 sunScaleMatrix;
    QMatrix4x4 sunRotateMatrix;
    QMatrix4x4 earthOrbitMatrix, moonOrbitMatrix, merkurOrbitMatrix, jupiterOrbitMatrix;
    QMatrix4x4 earthSelfRotateMatrix, moonSelfRotateMatrix, merkurSelfRotateMatrix, jupiterSelfRotateMatrix;
    float sunScaleMultp;
    float earthOrbitAngle, moonOrbitAngle, merkurOrbitAngle, jupiterOrbitAngle;
    float earthSelfRotateAngle, moonSelfRotateAngle, merkurSelfRotateAngle, jupiterSelfRotateAngle;

    GLuint sunSize;
    GLuint moonSize;



    // QOpenGLTexture *texture;
    GLuint textureID;

    QImage Texture;
    GLuint sunTexture, earthTexture, moonTexture, mercuryTexture, jupiterTexture;
};

#endif // QOPENGLPANEL_H
