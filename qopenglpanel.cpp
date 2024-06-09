#include "qopenglpanel.h"

QOpenGLPanel::QOpenGLPanel(QWidget *parent) :QOpenGLWidget(parent)
{
    resetScene();
}


void QOpenGLPanel::resetScene()
{
    translateMatrix.setToIdentity();
    rotateMatrix.setToIdentity();
    scaleMatrix.setToIdentity();



    tX = 0.0f, tY=0.0f, tZ=0.0f;
    rDegree = 0.0f, rX = 1.0f, rY=1.0f, rZ=1.0f;
    sX = 1.0f, sY=1.0f, sZ=1.0f;


    cameraMatrix.setToIdentity();
    camEyeX = 0.0f, camEyeY= 0.0f, camEyeZ = 5.0f;
    cameraEye = QVector3D(camEyeX, camEyeY, camEyeZ);
    camCenterX = 0.0f, camCenterY = 0.0f, camCenterZ = 0.0f;
    cameraCenter = QVector3D(camCenterX, camCenterY, camCenterZ);
    camUpX = 0.0f, camUpY = 1.0f, camUpZ = 0.0f;
    cameraUp = QVector3D(camUpX, camUpY, camUpZ);
    cameraMatrix.lookAt(cameraEye, cameraCenter, cameraUp);

    projectionMatrix.setToIdentity();
    verticalAngle = 90.0f;
    aspectRatio = float(this->width())/float(this->width());
    nearPlane = 1.0f;
    farPlane = 100.0f;
    projectionMatrix.perspective(verticalAngle, aspectRatio, nearPlane, farPlane);
}

QOpenGLPanel::~QOpenGLPanel()
{

}

void QOpenGLPanel::mousePressEvent(QMouseEvent* event)
{
    this->resetScene();
}


QOpenGLFunctions* QOpenGLPanel::getGLFunctions()
{
    return QOpenGLContext::currentContext()->functions();
}

QOpenGLExtraFunctions* QOpenGLPanel::getGLExtraFunctions()
{
    return QOpenGLContext::currentContext()->extraFunctions();
}

bool QOpenGLPanel::initializeShaderProgram(QString vertex, QString fragment, QOpenGLFunctions *f)
{
    progID = f->glCreateProgram();

    vertID = f->glCreateShader(GL_VERTEX_SHADER);
    const char* vertSource = readShaderSource(vertex);
    f->glShaderSource(vertID,1,&vertSource,nullptr);
    f->glCompileShader(vertID);
    f->glAttachShader(progID, vertID);

    fragID = f->glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragSource = readShaderSource(fragment);
    f->glShaderSource(fragID,1,&fragSource,nullptr);
    f->glCompileShader(fragID);
    f->glAttachShader(progID, fragID);

    f->glLinkProgram(progID);

    return checkGLError(f, "Linking Shader Program");
}

bool QOpenGLPanel::checkGLError(QOpenGLFunctions *f, QString functionCall)
{
    GLenum error = f->glGetError();
    if(error == GL_NO_ERROR){
        qDebug()<<"No OpenGL Error while "<<functionCall;
        return true;
    }else{
        qDebug()<<"Error "<<error<<" while "<<functionCall;
        return false;
    }
}

const char* QOpenGLPanel::readShaderSource(QString filename)
{
    const char* source = nullptr;
    QFile shaderFile(filename);
    if(!shaderFile.open(QFile::ReadOnly|QFile::Text)){
        qDebug()<<"Error while reading shader source file";
        return source;
    }

    QTextStream in(&shaderFile);
    QString text = in.readAll();
    shaderFile.close();
    QByteArray ba = text.toLocal8Bit();
    source = ba.data();

    return source;
}

void QOpenGLPanel::createSphere(GLuint X_SEGMENTS, GLuint Y_SEGMENTS, GLfloat x_offset, GLfloat y_offset, GLfloat z_offset)
{
    // sphereVAO = 0;
    //    unsigned int indexCount;


    vert.clear();
    indices.clear();

    positions.clear();
    uv.clear();
    normals.clear();

    /*
    ef->glGenVertexArrays(1, &sphereVAO);

    // unsigned int vbo, ebo;
    f->glGenBuffers(1, &vbo);
    f->glGenBuffers(1, &ebo);
    */

    /*
    QList<QVector3D> positions;
    QList<QVector2D> uv;
    QList<QVector3D> normals;

    std::vector<unsigned int> indices;
    */

    /*
    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    */

    const float PI = 3.14159265359f;
    for (GLuint x = 0; x <= X_SEGMENTS; ++x)
    {
        for (GLuint y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;

            float xPos = (cos(xSegment * 2.0f * PI) * sin(ySegment * PI)) + x_offset;
            float yPos = (cos(ySegment * PI)) + y_offset;
            float zPos = (sin(xSegment * 2.0f * PI) * sin(ySegment * PI)) + z_offset;

            QVector3D vertex(xPos, yPos, zPos);
            positions.append(vertex);

            QVector2D texture(xSegment, ySegment);
            uv.append(texture);

            QVector3D normal(xPos, yPos, zPos);
            normals.append(normal);
        }
    }

    bool oddRow = false;
    for (GLuint y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (GLuint x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    indexCount = static_cast<GLuint>(indices.size());


    for (GLuint i = 0; i < positions.size(); ++i)
    {
        vert.push_back(positions[i].x());
        vert.push_back(positions[i].y());
        vert.push_back(positions[i].z());
        if (normals.size() > 0)
        {
            vert.push_back(normals[i].x());
            vert.push_back(normals[i].y());
            vert.push_back(normals[i].z());
        }
        if (uv.size() > 0)
        {
            vert.push_back(uv[i].x());
            vert.push_back(uv[i].y());
        }
    }

    float vertAndColors[indices.size()];


    for (int i = 0; i < indices.size(); i++)
    {
        // qDebug() << i;
        // vertAndColors[i] = vertices[indices[i] - 1];
        vertAndColors[i] = vert[i];
    }

    /*
    ef->glBindVertexArray(sphereVAO);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    f->glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    unsigned int stride = (3 + 2 + 3) * sizeof(float);

    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    */

    /*
    for (int i = 0; i < positions.size(); ++i)
    {
        qDebug() << positions[i];
    }

    qDebug() << "data: " << &vert[0];
    */
}


void QOpenGLPanel::initializeGL()
{
    QOpenGLFunctions *f = getGLFunctions();
    f->initializeOpenGLFunctions();
    QOpenGLExtraFunctions *ef = getGLExtraFunctions();
    ef->initializeOpenGLFunctions();

    f->glClearColor(0.0, 0.4, 0.7, 0.0);

    // derinlik penceresini aktifleştir
    f->glEnable(GL_DEPTH_TEST);

    initializeShaderProgram(":simple.vert", ":simple.frag",f);

    translateMatrixID = f->glGetUniformLocation(progID,"translateMatrix");
    rotateMatrixID = f->glGetUniformLocation(progID, "rotateMatrix");
    scaleMatrixID = f->glGetUniformLocation(progID, "scaleMatrix");
    cameraMatrixID = f->glGetUniformLocation(progID, "cameraMatrix");
    projectionMatrixID = f->glGetUniformLocation(progID, "projectionMatrix");

    /*
    ef->glGenVertexArrays(2, &arrays);
    f->glGenBuffers(1,&triangleData);
    ef->glBindVertexArray(arrays);
    f->glBindBuffer(GL_ARRAY_BUFFER, triangleData);
    */

    //sphereVAO = 0;

    ef->glGenVertexArrays(2, &sphereVAO[0]);

    // unsigned int vbo, ebo;
    f->glGenBuffers(2, &vbo[0]);
    f->glGenBuffers(2, &ebo[0]);

    checkGLError(f, "Generating and Binding Vertex Arrays");

/*
    sphereVAO = 0;
//    unsigned int indexCount;

    if (sphereVAO == 0)
    {
        ef->glGenVertexArrays(1, &sphereVAO);

        // unsigned int vbo, ebo;
        f->glGenBuffers(1, &vbo);
        f->glGenBuffers(1, &ebo);

        QList<QVector3D> positions;
        QList<QVector2D> uv;
        QList<QVector3D> normals;

        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = cos(ySegment * PI);
                float zPos = sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                QVector3D vertex(xPos, yPos, zPos);
                positions.append(vertex);

                QVector2D texture(xSegment, ySegment);
                uv.append(texture);

                QVector3D normal(xPos, yPos, zPos);
                normals.append(normal);
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<unsigned int>(indices.size());


        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            vert.push_back(positions[i].x());
            vert.push_back(positions[i].y());
            vert.push_back(positions[i].z());
            if (normals.size() > 0)
            {
                vert.push_back(normals[i].x());
                vert.push_back(normals[i].y());
                vert.push_back(normals[i].z());
            }
            if (uv.size() > 0)
            {
                vert.push_back(uv[i].x());
                vert.push_back(uv[i].y());
            }
        }

        float vertAndColors[indices.size()];


        for (int i = 0; i < indices.size(); i++)
        {
            // qDebug() << i;
            // vertAndColors[i] = vertices[indices[i] - 1];
            vertAndColors[i] = vert[i];
        }

        ef->glBindVertexArray(sphereVAO);
        f->glBindBuffer(GL_ARRAY_BUFFER, vbo);
        f->glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

        f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        unsigned int stride = (3 + 2 + 3) * sizeof(float);

        f->glEnableVertexAttribArray(0);
        f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

        f->glEnableVertexAttribArray(1);
        f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

        f->glEnableVertexAttribArray(2);
        f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

        for (int i = 0; i < positions.size(); ++i)
        {
            qDebug() << positions[i];
        }

        qDebug() << "data: " << &vert[0];
    }
*/


    /*
    //f->glBufferData(GL_ARRAY_BUFFER, sizeof(vertAndColors), vertAndColors, GL_STATIC_DRAW);

    position = f->glGetAttribLocation(progID, "position");
    f->glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)0);
    f->glEnableVertexAttribArray(position);

    color = f->glGetAttribLocation(progID, "color");
    f->glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(sizeof(float)*3));
    f->glEnableVertexAttribArray(color);
    */

    GLuint stride = (3 + 2 + 3) * sizeof(float);

    createSphere(64, 64, 0.0f, 0.0f, 0.0f);

    ef->glBindVertexArray(sphereVAO[0]);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    f->glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // unsigned int stride = (3 + 2 + 3) * sizeof(float);

    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));



    createSphere(64, 64, 0.0f, 0.0f, 0.0f);

    // Güneş için dönüş matrisini başlat
    sunRotateMatrix.setToIdentity();

    // Dünya için yörünge ve kendi ekseni etrafında dönüş matrislerini başlat
    earthOrbitMatrix.setToIdentity();
    earthSelfRotateMatrix.setToIdentity();
    earthOrbitAngle = 0.0f;
    earthSelfRotateAngle = 0.0f;

    // Ay için yörünge ve kendi ekseni etrafında dönüş matrislerini başlat
    moonOrbitMatrix.setToIdentity();
    moonSelfRotateMatrix.setToIdentity();
    moonOrbitAngle = 0.0f;
    moonSelfRotateAngle = 0.0f;


    ef->glBindVertexArray(sphereVAO[1]);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    f->glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

}

void QOpenGLPanel::translate(float x, float y, float z)
{
    tX=x, tY=y, tZ=z;
    translateMatrix.translate(tX,tY,tZ);
}

void QOpenGLPanel::rotate(float degree, float x, float y, float z)
{
    rDegree+=degree;
    rotateMatrix.rotate(rDegree, x, y, z);
}

void QOpenGLPanel::scale(float x, float y, float z)
{
    sX=x, sY=y, sZ=z;
    scaleMatrix.scale(sX,sY,sZ);
}

void QOpenGLPanel::lookAt( float ex,  float ey,  float ez,  float cx,  float cy,  float cz, float ux,  float uy, float uz)
{
    camEyeX = ex, camEyeY= ey, camEyeZ = ez;
    cameraEye = QVector3D(camEyeX, camEyeY, camEyeZ);
    camCenterX = cx, camCenterY = cy, camCenterZ = cz;
    cameraCenter = QVector3D(camCenterX, camCenterY, camCenterZ);
    camUpX = ux, camUpY = uy, camUpZ = uz;
    cameraUp = QVector3D(camUpX, camUpY, camUpZ);
    cameraMatrix.lookAt(cameraEye, cameraCenter, cameraUp);
}

void QOpenGLPanel::perspective( float angle,  float ratio,  float nearp,  float farp)
{
    verticalAngle = angle;
    aspectRatio = ratio;
    nearPlane = nearp, farPlane = farp;
    projectionMatrix.perspective(verticalAngle, aspectRatio, nearPlane, farPlane);
}


void QOpenGLPanel::paintGL()
{
    QOpenGLFunctions *f = getGLFunctions();
    QOpenGLExtraFunctions *ef = getGLExtraFunctions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    f->glUseProgram(progID);

    /*
    f->glUniformMatrix4fv(translateMatrixID,1,GL_FALSE,translateMatrix.constData());
    f->glUniformMatrix4fv(rotateMatrixID,1,GL_FALSE,rotateMatrix.constData());
    f->glUniformMatrix4fv(scaleMatrixID,1,GL_FALSE,scaleMatrix.constData());
    f->glUniformMatrix4fv(cameraMatrixID,1,GL_FALSE,cameraMatrix.constData());
    f->glUniformMatrix4fv(projectionMatrixID,1,GL_FALSE,projectionMatrix.constData());
    */

    f->glUniformMatrix4fv(translateMatrixID,1,GL_FALSE,translateMatrix.constData());
    f->glUniformMatrix4fv(scaleMatrixID,1,GL_FALSE,scaleMatrix.constData());
    f->glUniformMatrix4fv(cameraMatrixID,1,GL_FALSE,cameraMatrix.constData());
    f->glUniformMatrix4fv(projectionMatrixID,1,GL_FALSE,projectionMatrix.constData());

    /*
    ef->glBindVertexArray(arrays);
    f->glDrawArrays(GL_TRIANGLES, 0, 60);
    */

    // güneş çizimi
    //f->glUniformMatrix4fv(rotateMatrixID,1,GL_FALSE,rotateMatrix.constData());
    f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, sunRotateMatrix.constData());
    ef->glBindVertexArray(sphereVAO[0]);
    f->glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);

    /*
    secondSphereRotateAngle += 0.5f; // Her çerçevede 0.5 derece döndür

    // Dönüş matrisini güncelleyin
    secondSphereRotateMatrix.setToIdentity();
    secondSphereRotateMatrix.rotate(secondSphereRotateAngle, 0.0f, 1.0f, 0.0f); // Y ekseninde döndür
    */

    // Dünya için dönüş açılarını güncelleyin
    earthOrbitAngle += 0.5f; // Dünya'nın Güneş etrafında yörünge hareketi
    earthSelfRotateAngle += 1.0f; // Dünya'nın kendi ekseni etrafında dönüşü

    /*
    // Çeviri matrisini güncelleyin (küreyi birinci küreden uzaklaştırarak yörüngede döndürün)
    secondSphereTranslateMatrix.setToIdentity();
    secondSphereTranslateMatrix.translate(5.0f, 0.0f, 0.0f); // 5 birim sağa kaydır (örnek olarak)
    */

    /*
    // Çeviri ve dönüş matrislerini birleştir
    QMatrix4x4 secondSphereModelMatrix = secondSphereTranslateMatrix * secondSphereRotateMatrix;

    f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, secondSphereModelMatrix.constData());
    */

    // Dünya'nın yörünge matrisini güncelleyin
    earthOrbitMatrix.setToIdentity();
    earthOrbitMatrix.rotate(earthOrbitAngle, 0.0f, 1.0f, 0.0f); // Y ekseninde döndür
    earthOrbitMatrix.translate(5.0f, 0.0f, 0.0f); // 5 birim sağa kaydır (örnek olarak yörünge yarıçapı)

    // Dünya'nın kendi ekseni etrafında dönüş matrisini güncelleyin
    earthSelfRotateMatrix.setToIdentity();
    earthSelfRotateMatrix.rotate(earthSelfRotateAngle, 0.0f, 1.0f, 0.0f); // Y ekseninde döndür


    // Dünya'nın model matrisini oluşturun ve çizin
    QMatrix4x4 earthModelMatrix = earthOrbitMatrix * earthSelfRotateMatrix;
    f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, earthModelMatrix.constData());

    ef->glBindVertexArray(sphereVAO[0]);
    f->glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);



    // Dünya için dönüş açılarını güncelleyin
    moonOrbitAngle += 1.0f; // Dünya'nın Güneş etrafında yörünge hareketi
    moonSelfRotateAngle += 1.0f; // Dünya'nın kendi ekseni etrafında dönüşü

    // Ay'ın yörünge matrisini güncelleyin
    moonOrbitMatrix.setToIdentity();
    moonOrbitMatrix.rotate(moonOrbitAngle, 0.0f, 1.0f, 0.0f); // Y ekseninde döndür
    moonOrbitMatrix.translate(-2.5f, 0.0f, 0.0f); // 5 birim sağa kaydır (örnek olarak yörünge yarıçapı)

    // Ay'ın kendi ekseni etrafında dönüş matrisini güncelleyin
    moonSelfRotateMatrix.setToIdentity();
    moonSelfRotateMatrix.rotate(moonSelfRotateAngle, 0.0f, 1.0f, 0.0f); // Y ekseninde döndür


    // Dünya'nın model matrisini oluşturun ve çizin
    QMatrix4x4 moonMatrix = moonOrbitMatrix * moonSelfRotateMatrix;
    f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, moonMatrix.constData());

    ef->glBindVertexArray(sphereVAO[0]);
    f->glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);

    /*
    // Çeviri matrisini güncelleyin (küreyi birinci küreden uzaklaştırarak yörüngede döndürün)
    secondSphereTranslateMatrix.setToIdentity();
    secondSphereTranslateMatrix.translate(-5.0f, 0.0f, 0.0f); // 5 birim sağa kaydır (örnek olarak)


    QMatrix4x4 thirdSphereModelMatrix = secondSphereTranslateMatrix * secondSphereRotateMatrix;
    */

    /*
    // Dünya'nın model matrisini oluşturun ve çizin
    QMatrix4x4 earthModelMatrix = earthOrbitMatrix * earthSelfRotateMatrix;

    // Çeviri ve dönüş matrislerini birleştir
    // f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, thirdSphereModelMatrix.constData());
    f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, earthModelMatrix.constData());
    ef->glBindVertexArray(sphereVAO[1]);
    f->glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    */

    update();
}

void QOpenGLPanel::resizeGL(int width, int height)
{


}
