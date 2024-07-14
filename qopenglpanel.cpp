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
    camEyeX = 20.0f, camEyeY= 50.0f, camEyeZ = 80.0f;
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
    farPlane = 200.0f;
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

// Kure olusturma fonksiyonu
// Bu fonksiyon, kure olusturma uzerine yaptigimiz uzun sureli arastirmalar sonucunda internetteki bir çalışmadan esinlenilmiştir.
// Bu fonksiyon uzerine bircok duzenleme ve ekleme yaptik.
GLuint QOpenGLPanel::createSphere(GLuint X_SEGMENTS, GLuint Y_SEGMENTS, GLfloat x_offset, GLfloat y_offset, GLfloat z_offset)
{
    // vectorler temizleniyor.
    vert.clear();
    indices.clear();

    positions.clear();
    uv.clear();
    normals.clear();

    // kurenin dikey ve yataydaki cozunurluklerine gore vertex, texture ve normal bilgileri hesaplaniyor.
    // hesaplanan vertex bilgileri, fonksiyona girilen (x_offset, y_offset ve z_offset) bilgileriyle uzayda istenilen yere ötelenebilmektedir.
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

            // hesaplanan vertex bilgileri 3 boyutlu koordinata donusturuluyor ve positions arrayine ekleniyor
            QVector3D vertex(xPos, yPos, zPos);
            positions.append(vertex);

            // hesaplanan texture bilgileri 2 boyutlu koordinata donusturuluyor ve uv arrayine ekleniyor
            QVector2D texture(xSegment, ySegment);
            uv.append(texture);

            // hesaplanan normal bilgileri 3 boyutlu koordinata donusturuluyor ve normals arrayine ekleniyor
            QVector3D normal(xPos, yPos, zPos);
            normals.append(normal);
        }
    }

    // kurenin, ucgenlerle cizilebilmesi icin her satırdaki ucgenleri saglayan vertexlerin indexleri olusturuluyor.
    bool oddRow = false;
    for (GLuint y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow)
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

    // indexlerin sayısı tutuluyor.
    indexCount = static_cast<GLuint>(indices.size());

    // vertex positionları, normal positionları ve texture positionları aynı dizide birlestiriliyor.
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

    // farklı kureler olusturulursa her kureyi farklı farklı cizdirebilmek icin olusturulan kurelerin index sayilari return ediliyor.
    return indexCount;
}

// dosya yoluna ve ismine gore texture dosyalari gerekli texture bufferina yukler.
GLuint QOpenGLPanel::loadTexture(QString fileName){

    // opengl functionlari tanimlaniyor
    QOpenGLFunctions *f = getGLFunctions();
    f->initializeOpenGLFunctions();
    QOpenGLExtraFunctions *ef = getGLExtraFunctions();
    ef->initializeOpenGLFunctions();

    // hedef dosya resim haline getiriliyor
    Texture = QImage(fileName);

    // belirtilen dosya hedefte yoksa donduruyor.
    if (Texture.isNull()) {
        qDebug() << "Failed to load texture from" << fileName;
        return -1;
    }

    GLuint textureData;

    // texture buffer hazırlanıyor ve texture buffera ekleniyor.
    f->glGenTextures(1, &textureData);
    f->glBindTexture(GL_TEXTURE_2D, textureData);

    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // nesnenin pikselleri 24 bit bir rgb formatina donduruluyor.
    QImage im = Texture.convertToFormat(QImage::Format_RGB888);

    // 2d texture olusturur ve gpu ya yukler.
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Texture.width(), Texture.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, im.bits());
    f->glGenerateMipmap(GL_TEXTURE_2D);

    // texture bufferi return ediliyor.
    return textureData;
}

void QOpenGLPanel::initializeGL()
{
    QOpenGLFunctions *f = getGLFunctions();
    f->initializeOpenGLFunctions();
    QOpenGLExtraFunctions *ef = getGLExtraFunctions();
    ef->initializeOpenGLFunctions();

    f->glClearColor(0.0, 0.0, 0.0, 0.0);

    // derinlik penceresini aktifleştirir
    f->glEnable(GL_DEPTH_TEST);

    initializeShaderProgram(":simple.vert", ":simple.frag",f);

    translateMatrixID = f->glGetUniformLocation(progID,"translateMatrix");
    rotateMatrixID = f->glGetUniformLocation(progID, "rotateMatrix");
    scaleMatrixID = f->glGetUniformLocation(progID, "scaleMatrix");
    cameraMatrixID = f->glGetUniformLocation(progID, "cameraMatrix");
    projectionMatrixID = f->glGetUniformLocation(progID, "projectionMatrix");

    // bufferlar aktiflestiriliyor.
    ef->glGenVertexArrays(2, &sphereVAO[0]);

    f->glGenBuffers(2, &vbo[0]);
    f->glGenBuffers(2, &ebo[0]);


    checkGLError(f, "Generating and Binding Vertex Arrays");


    // güneş texture u ekleniyor
    QString sunTexFileName = ":/img/8k_sun.jpg";
    sunTexture = loadTexture(sunTexFileName);

    GLuint stride = (3 + 2 + 3) * sizeof(float);

    // ana kure olusturuluyor. (Tum gezegenler bu kureyi kullanıyor.)
    sunSize = createSphere(64, 64, 0.0f, 0.0f, 0.0f);

    // tüm vertex bilgileri (vertex, texture ve normal) buffera ekleniyor.
    ef->glBindVertexArray(sphereVAO[0]);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    f->glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

    // indices ler buffera yukleniyor
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


    position = f->glGetAttribLocation(progID, "position");
    f->glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    f->glEnableVertexAttribArray(position);


    texture = f->glGetAttribLocation(progID, "aTexCoord");
    f->glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    f->glEnableVertexAttribArray(texture);


    // dünya texture u ekleniyor
    QString earthTexFileName = ":/img/earth2048.bmp";
    earthTexture = loadTexture(earthTexFileName);

    // ay texture u ekleniyor
    QString moonTexFileName = ":/img/moon1024.bmp";
    moonTexture = loadTexture(moonTexFileName);

    //merkür
    QString mercuryTexFileName = ":/img/2k_mercury.jpg";
    mercuryTexture = loadTexture(mercuryTexFileName);

    //venus
    QString venusTexFileName = ":/img/2k_venus_surface.jpg";
    venusTexture = loadTexture(venusTexFileName);

    //mars
    QString marsTexFileName = ":/img/2k_mars.jpg";
    marsTexture = loadTexture(marsTexFileName);

    //jupiter
    QString jupiterTexFileName = ":/img/2k_jupiter.jpg";
    jupiterTexture = loadTexture(jupiterTexFileName);

    //saturn
    QString saturnTexFileName = ":/img/saturn.jpg";
    saturnTexture = loadTexture(saturnTexFileName);

    //uranus
    QString uranusTexFileName = ":/img/2k_uranus.jpg";
    uranusTexture = loadTexture(uranusTexFileName);

    //neptune
    QString neptuneTexFileName = ":/img/2k_neptune.jpg";
    neptuneTexture = loadTexture(neptuneTexFileName);

    // ay icin kure olusturuluyor (bu ekstra olusturulan kure, birden fazla farklı kureyi kullanabilmenin yolunu gostermektedir.)
    moonSize = createSphere(64, 64, 0.0f, 0.0f, 0.0f);

    ef->glBindVertexArray(sphereVAO[1]);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    f->glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


    position = f->glGetAttribLocation(progID, "position");
    f->glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    f->glEnableVertexAttribArray(position);

    texture = f->glGetAttribLocation(progID, "aTexCoord");
    f->glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    f->glEnableVertexAttribArray(texture);

    // Güneş için dönüş matrisini başlat
    sunRotateMatrix.setToIdentity();
    sunScaleMatrix.setToIdentity();

    // Güneşin buyukluk çarpanı
    sunScaleMultp = 0.0f;

    // Dünya için yörünge ve kendi ekseni etrafında dönüş matrislerini başlat
    earthScaleMatrix.setToIdentity();
    earthOrbitMatrix.setToIdentity(); // güneş etrafında dönme matriksi
    earthSelfRotateMatrix.setToIdentity();  // kendi etrafında dönme matriksi
    earthOrbitAngle = 0.0f; // dünya etrafında dönme açısı
    earthSelfRotateAngle = 0.0f;    // kendi etrafında dönme açısı

    // Ay için yörünge ve kendi ekseni etrafında dönüş matrislerini başlat
    moonScaleMatrix.setToIdentity();
    moonOrbitMatrix.setToIdentity();
    moonSelfRotateMatrix.setToIdentity();
    moonOrbitAngle = 0.0f;
    moonSelfRotateAngle = 0.0f;

    //merkür
    // için yörünge ve kendi ekseni etrafında dönüş matrislerini başlat
    merkurScaleMatrix.setToIdentity();
    merkurOrbitMatrix.setToIdentity(); // güneş etrafında dönme matriksi
    merkurSelfRotateMatrix.setToIdentity();  // kendi etrafında dönme matriksi
    merkurOrbitAngle = 0.0f; // etrafında dönme açısı
    merkurSelfRotateAngle = 0.0f;    // kendi etrafında dönme açısı

    //venüs
    // için yörünge ve kendi ekseni etrafında dönüş matrislerini başlat
    venusScaleMatrix.setToIdentity();
    venusOrbitMatrix.setToIdentity(); // güneş etrafında dönme matriksi
    venusSelfRotateMatrix.setToIdentity();  // kendi etrafında dönme matriksi
    venusOrbitAngle = 0.0f; // etrafında dönme açısı
    venusSelfRotateAngle = 0.0f;    // kendi etrafında dönme açısı

    //mars
    // için yörünge ve kendi ekseni etrafında dönüş matrislerini başlat
    marsScaleMatrix.setToIdentity();
    marsOrbitMatrix.setToIdentity(); // güneş etrafında dönme matriksi
    marsSelfRotateMatrix.setToIdentity();  // kendi etrafında dönme matriksi
    marsOrbitAngle = 0.0f; // etrafında dönme açısı
    marsSelfRotateAngle = 0.0f;    // kendi etrafında dönme açısı

    //jupiter
    // için yörünge ve kendi ekseni etrafında dönüş matrislerini başlat
    jupiterScaleMatrix.setToIdentity();
    jupiterOrbitMatrix.setToIdentity(); // güneş etrafında dönme matriksi
    jupiterSelfRotateMatrix.setToIdentity();  // kendi etrafında dönme matriksi
    jupiterOrbitAngle = 0.0f; // etrafında dönme açısı
    jupiterSelfRotateAngle = 0.0f;    // kendi etrafında dönme açısı

    //satürn
    // için yörünge ve kendi ekseni etrafında dönüş matrislerini başlat
    saturnScaleMatrix.setToIdentity();
    saturnOrbitMatrix.setToIdentity(); // güneş etrafında dönme matriksi
    saturnSelfRotateMatrix.setToIdentity();  // kendi etrafında dönme matriksi
    saturnOrbitAngle = 0.0f; // etrafında dönme açısı
    saturnSelfRotateAngle = 0.0f;    // kendi etrafında dönme açısı

    //uranus
    // için yörünge ve kendi ekseni etrafında dönüş matrislerini başlat
    uranusScaleMatrix.setToIdentity();
    uranusOrbitMatrix.setToIdentity(); // güneş etrafında dönme matriksi
    uranusSelfRotateMatrix.setToIdentity();  // kendi etrafında dönme matriksi
    uranusOrbitAngle = 0.0f; // etrafında dönme açısı
    uranusSelfRotateAngle = 0.0f;    // kendi etrafında dönme açısı

    //neptune
    // için yörünge ve kendi ekseni etrafında dönüş matrislerini başlat
    neptuneScaleMatrix.setToIdentity();
    neptuneOrbitMatrix.setToIdentity(); // güneş etrafında dönme matriksi
    neptuneSelfRotateMatrix.setToIdentity();  // kendi etrafında dönme matriksi
    neptuneOrbitAngle = 0.0f; // etrafında dönme açısı
    neptuneSelfRotateAngle = 0.0f;    // kendi etrafında dönme açısı


    checkGLError(f, "Enabling and Setting Vertex Attributes");
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

    f->glUniformMatrix4fv(translateMatrixID,1,GL_FALSE,translateMatrix.constData());
    f->glUniformMatrix4fv(scaleMatrixID,1,GL_FALSE,scaleMatrix.constData());
    f->glUniformMatrix4fv(cameraMatrixID,1,GL_FALSE,cameraMatrix.constData());
    f->glUniformMatrix4fv(projectionMatrixID,1,GL_FALSE,projectionMatrix.constData());

    {
        // Güneş işlemleri

        sunScaleMatrix.setToIdentity();
        sunScaleMultp = 5.0f;
        sunScaleMatrix.scale(sunScaleMultp, sunScaleMultp, sunScaleMultp);

        sunSelfRotateAngle += 0.5f;
        sunRotateMatrix.setToIdentity(); // matrixi tanımlar
        sunRotateMatrix.rotate(sunSelfRotateAngle, 0.0f, 1.0f, 0.0f); // Yörüngede Y ekseninde döndür

        // gunesin hem kendi etrafında donmesini, hem de arayuzdeki donme ile donmesini saglar
        QMatrix4x4 sunModelMatrix = sunRotateMatrix * rotateMatrix;
        f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, sunModelMatrix.constData());

        // güneş çizimi
        f->glUniformMatrix4fv(scaleMatrixID,1,GL_FALSE,sunScaleMatrix.constData());

        // güneş texture u etkinleştiriliyor
        f->glBindTexture(GL_TEXTURE_2D, sunTexture);

        ef->glBindVertexArray(sphereVAO[0]);
        f->glDrawElements(GL_TRIANGLE_STRIP, sunSize, GL_UNSIGNED_INT, 0);
    }

    {
        // Merkür işlemleri

        merkurScaleMatrix.setToIdentity();
        float MerkurScaleMulp = 0.9f;
        merkurScaleMatrix.scale(MerkurScaleMulp, MerkurScaleMulp, MerkurScaleMulp);



        // için dönüş açılarını günceller
        merkurOrbitAngle += 1.0f; // Güneş etrafında yörünge hareketi
        merkurSelfRotateAngle += 0.5f; //kendi ekseni etrafında dönüşü


        //yörünge matrisini günceller
        merkurOrbitMatrix.setToIdentity();
        merkurOrbitMatrix.rotate(merkurOrbitAngle, 0.0f, 5.0f, 0.0f); // Yörüngede Y ekseninde döndür
        merkurOrbitMatrix.translate(0.39f * 20.0f, 0.0f, 0.0f);

        // kendi ekseni etrafında dönüş matrisini günceller
        merkurSelfRotateMatrix.setToIdentity();
        merkurSelfRotateMatrix.rotate(merkurSelfRotateAngle, 0.0f, 1.0f, 0.0f); // Kendi etrafında Y ekseninde döndür


        // model matrisini oluşturuluyor ve çiziliyor
        QMatrix4x4 merkurModelMatrix = merkurOrbitMatrix * merkurSelfRotateMatrix * rotateMatrix;
        f->glUniformMatrix4fv(scaleMatrixID,1,GL_FALSE,merkurScaleMatrix.constData());
        f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, merkurModelMatrix.constData());


        // texture u etkinleştiriliyor
        f->glBindTexture(GL_TEXTURE_2D, mercuryTexture);

        ef->glBindVertexArray(sphereVAO[0]);
        f->glDrawElements(GL_TRIANGLE_STRIP, sunSize, GL_UNSIGNED_INT, 0);
    }

    {
        // Venus işlemleri
        venusScaleMatrix.setToIdentity();
        float venusScaleMulp = 1.0f;
        venusScaleMatrix.scale(venusScaleMulp, venusScaleMulp, venusScaleMulp);

        // için dönüş açılarını günceller
        venusOrbitAngle += 0.9f; // Güneş etrafında yörünge hareketi
        venusSelfRotateAngle += 0.8f; //kendi ekseni etrafında dönüşü


        // yörünge matrisini günceller
        venusOrbitMatrix.setToIdentity();
        venusOrbitMatrix.rotate(venusOrbitAngle, 0.0f, 1.0f, 0.0f); // Yörüngede Y ekseninde döndür
        venusOrbitMatrix.translate(0.7f + 22.0f, 0.0f, 0.0f);

        // kendi ekseni etrafında dönüş matrisini günceller
        venusSelfRotateMatrix.setToIdentity();
        venusSelfRotateMatrix.rotate(merkurSelfRotateAngle, 0.0f, 1.0f, 0.0f); // Kendi etrafında Y ekseninde döndür


        // model matrisini oluşturuluyor ve çiziliyor
        QMatrix4x4 venusModelMatrix = venusOrbitMatrix * venusSelfRotateMatrix * rotateMatrix;
        f->glUniformMatrix4fv(scaleMatrixID,1,GL_FALSE,venusScaleMatrix.constData());
        f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, venusModelMatrix.constData());

        // texture u etkinleştiriliyor
        f->glBindTexture(GL_TEXTURE_2D, venusTexture);

        ef->glBindVertexArray(sphereVAO[0]);
        f->glDrawElements(GL_TRIANGLE_STRIP, sunSize, GL_UNSIGNED_INT, 0);
    }


    {
        // Dünya işlemleri
        earthScaleMatrix.setToIdentity();
        float earthScaleMulp = 1.0f;
        earthScaleMatrix.scale(earthScaleMulp, earthScaleMulp, earthScaleMulp);

        // Dünya için dönüş açılarını günceller
        earthOrbitAngle += 0.8f; // Dünya'nın Güneş etrafında yörünge hareketi
        earthSelfRotateAngle += 1.3f; // Dünya'nın kendi ekseni etrafında dönüşü


        // Dünya'nın yörünge matrisini günceller
        earthOrbitMatrix.setToIdentity();
        earthOrbitMatrix.rotate(earthOrbitAngle, 0.0f, 1.0f, 0.0f); // Yörüngede Y ekseninde döndür
        earthOrbitMatrix.translate(-1.0f * 25.0f, 0.0f);

        // Dünya'nın kendi ekseni etrafında dönüş matrisini günceller
        earthSelfRotateMatrix.setToIdentity();
        earthSelfRotateMatrix.rotate(earthSelfRotateAngle, 0.0f, 1.0f, 0.0f); // Kendi etrafında Y ekseninde döndür


        // Dünya'nın model matrisini oluşturuluyor ve çiziliyor
        QMatrix4x4 earthModelMatrix = earthOrbitMatrix * earthSelfRotateMatrix * rotateMatrix;
        f->glUniformMatrix4fv(scaleMatrixID,1,GL_FALSE,earthScaleMatrix.constData());
        f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, earthModelMatrix.constData());

        // dünya texture u etkinleştiriliyor
        f->glBindTexture(GL_TEXTURE_2D, earthTexture);

        ef->glBindVertexArray(sphereVAO[0]);
        f->glDrawElements(GL_TRIANGLE_STRIP, sunSize, GL_UNSIGNED_INT, 0);
    }


    {
        // Ay işlemleri
        moonScaleMatrix.setToIdentity();
        float moonScaleMulp = 0.4f;
        moonScaleMatrix.scale(moonScaleMulp, moonScaleMulp, moonScaleMulp);

        // Ay için dönüş açılarını güncelleniyor
        moonOrbitAngle += 1.0f; // Ay'ın Dünya etrafında yörünge hareketi
        moonSelfRotateAngle += 0.5f; // Ay'ın kendi ekseni etrafında dönüşü

        // Ay'ın yörünge matrisini güncelleniyor
        moonOrbitMatrix.setToIdentity();
        moonOrbitMatrix.rotate(moonOrbitAngle, 0.0f, 1.0f, 0.0f); // Y ekseninde döndür
        moonOrbitMatrix.translate(3.0f, 0.0f, 0.0f);

        // Ay'ın kendi ekseni etrafında dönüş matrisini güncelleniyor
        moonSelfRotateMatrix.setToIdentity();
        moonSelfRotateMatrix.rotate(moonSelfRotateAngle, 0.0f, 1.0f, 0.0f); // Y ekseninde döndür


        // Dünya'nın model matrisini oluşturuluyor ve çiziliyoe
        QMatrix4x4 moonMatrix = earthOrbitMatrix * moonOrbitMatrix * moonSelfRotateMatrix * rotateMatrix;
        f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, moonMatrix.constData());
        f->glUniformMatrix4fv(scaleMatrixID,1,GL_FALSE,moonScaleMatrix.constData());

        // ay texture u etkinleştiriliyor
        f->glBindTexture(GL_TEXTURE_2D, moonTexture);

        ef->glBindVertexArray(sphereVAO[1]);
        f->glDrawElements(GL_TRIANGLE_STRIP, moonSize, GL_UNSIGNED_INT, 0);
    }

    {
        // Mars işlemleri

        marsScaleMatrix.setToIdentity();
        float marsScaleMulp = 1.0f;
        marsScaleMatrix.scale(marsScaleMulp, marsScaleMulp, marsScaleMulp);

        // için dönüş açılarını günceller
        marsOrbitAngle += 0.5f; // Güneş etrafında yörünge hareketi
        marsSelfRotateAngle += 1.0f; //kendi ekseni etrafında dönüşü


        // yörünge matrisini günceller
        marsOrbitMatrix.setToIdentity();
        marsOrbitMatrix.rotate(marsOrbitAngle, 0.0f, 1.0f, 0.0f); // Yörüngede Y ekseninde döndür
        marsOrbitMatrix.translate(-1.0f * 35.0f, 0.0f, 0.0f);

        // kendi ekseni etrafında dönüş matrisini günceller
        marsSelfRotateMatrix.setToIdentity();
        marsSelfRotateMatrix.rotate(marsSelfRotateAngle, 0.0f, 1.0f, 0.0f); // Kendi etrafında Y ekseninde döndür


        // model matrisini oluşturuluyor ve çiziliyor
        QMatrix4x4 marsModelMatrix = marsOrbitMatrix * marsSelfRotateMatrix * rotateMatrix;
        f->glUniformMatrix4fv(scaleMatrixID,1,GL_FALSE,marsScaleMatrix.constData());
        f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, marsModelMatrix.constData());

        // texture u etkinleştiriliyor
        f->glBindTexture(GL_TEXTURE_2D, marsTexture);

        ef->glBindVertexArray(sphereVAO[0]);
        f->glDrawElements(GL_TRIANGLE_STRIP, sunSize, GL_UNSIGNED_INT, 0);
    }




    {
        // jupiter işlemleri

        jupiterScaleMatrix.setToIdentity();
        float jupiteScaleMulp = 2.5f;
        jupiterScaleMatrix.scale(jupiteScaleMulp, jupiteScaleMulp, jupiteScaleMulp);

        // dönüş açılarını günceller
        jupiterOrbitAngle += 0.4f;
        jupiterSelfRotateAngle += 3.0f;


        // yörünge matrisini günceller
        jupiterOrbitMatrix.setToIdentity();
        jupiterOrbitMatrix.rotate(jupiterOrbitAngle, 0.0f, 1.0f, 0.0f); // Yörüngede Y ekseninde döndür
        jupiterOrbitMatrix.translate(1.0f * 48.0f, 0.0f, 0.0f);

        // jupiter'in kendi ekseni etrafında dönüş matrisini günceller
        jupiterSelfRotateMatrix.setToIdentity();
        jupiterSelfRotateMatrix.rotate(jupiterSelfRotateAngle, 0.0f, 1.0f, 0.0f); // Kendi etrafında Y ekseninde döndür


        //  model matrisini oluşturuluyor ve çiziliyor
        QMatrix4x4 jupiterModelMatrix = jupiterOrbitMatrix * jupiterSelfRotateMatrix * rotateMatrix;
        f->glUniformMatrix4fv(scaleMatrixID,1,GL_FALSE,jupiterScaleMatrix.constData());
        f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, jupiterModelMatrix.constData());

        //  texture u etkinleştiriliyor
        f->glBindTexture(GL_TEXTURE_2D, jupiterTexture);

        ef->glBindVertexArray(sphereVAO[0]);
        f->glDrawElements(GL_TRIANGLE_STRIP, sunSize, GL_UNSIGNED_INT, 0);
    }

    {
        // saturn işlemleri

        saturnScaleMatrix.setToIdentity();
        float saturnScaleMulp = 2.0f;
        saturnScaleMatrix.scale(saturnScaleMulp, saturnScaleMulp, saturnScaleMulp);

        //  için dönüş açılarını günceller
        saturnOrbitAngle += 0.3f; //  Güneş etrafında yörünge hareketi
        saturnSelfRotateAngle += 2.7f; //  kendi ekseni etrafında dönüşü


        // yörünge matrisini günceller
        saturnOrbitMatrix.setToIdentity();
        saturnOrbitMatrix.rotate(saturnOrbitAngle, 0.0f, 1.0f, 0.0f); // Yörüngede Y ekseninde döndür
        saturnOrbitMatrix.translate(-1.0f * 58.0f, 0.0f, 0.0f);

        //  kendi ekseni etrafında dönüş matrisini günceller
        saturnSelfRotateMatrix.setToIdentity();
        saturnSelfRotateMatrix.rotate(jupiterSelfRotateAngle, 0.0f, 1.0f, 0.0f); // Kendi etrafında Y ekseninde döndür


        // model matrisini oluşturuluyor ve çiziliyor
        QMatrix4x4 saturnModelMatrix = saturnOrbitMatrix * saturnSelfRotateMatrix * rotateMatrix;
        f->glUniformMatrix4fv(scaleMatrixID,1,GL_FALSE,saturnScaleMatrix.constData());
        f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, saturnModelMatrix.constData());

        // texture u etkinleştiriliyor
        f->glBindTexture(GL_TEXTURE_2D, saturnTexture);

        ef->glBindVertexArray(sphereVAO[0]);
        f->glDrawElements(GL_TRIANGLE_STRIP, sunSize, GL_UNSIGNED_INT, 0);
    }


    {
        // uranus işlemleri
        uranusScaleMatrix.setToIdentity();
        float uranusScaleMulp = 1.5f;
        uranusScaleMatrix.scale(uranusScaleMulp, uranusScaleMulp, uranusScaleMulp);

        //  için dönüş açılarını günceller
        uranusOrbitAngle += 0.2f; //  Güneş etrafında yörünge hareketi
        uranusSelfRotateAngle += 2.0f; //  kendi ekseni etrafında dönüşü


        // yörünge matrisini günceller
        uranusOrbitMatrix.setToIdentity();
        uranusOrbitMatrix.rotate(uranusOrbitAngle, 0.0f, 1.0f, 0.0f); // Yörüngede Y ekseninde döndür
        uranusOrbitMatrix.translate(1.0f * 68.0f, 0.0f, 0.0f);

        //  kendi ekseni etrafında dönüş matrisini günceller
        uranusSelfRotateMatrix.setToIdentity();
        uranusSelfRotateMatrix.rotate(uranusSelfRotateAngle, 0.0f, 1.0f, 0.0f); // Kendi etrafında Y ekseninde döndür


        // model matrisini oluşturuluyor ve çiziliyor
        QMatrix4x4 uranusModelMatrix = uranusOrbitMatrix * uranusSelfRotateMatrix * rotateMatrix;
        f->glUniformMatrix4fv(scaleMatrixID,1,GL_FALSE,uranusScaleMatrix.constData());
        f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, uranusModelMatrix.constData());

        // texture u etkinleştiriliyor
        f->glBindTexture(GL_TEXTURE_2D, uranusTexture);

        ef->glBindVertexArray(sphereVAO[0]);
        f->glDrawElements(GL_TRIANGLE_STRIP, sunSize, GL_UNSIGNED_INT, 0);
    }


    {
        // neptune işlemleri
        neptuneScaleMatrix.setToIdentity();
        float neptuneScaleMulp = 1.5f;
        neptuneScaleMatrix.scale(neptuneScaleMulp, neptuneScaleMulp, neptuneScaleMulp);

        //  için dönüş açılarını günceller
        neptuneOrbitAngle += 0.1f; //  Güneş etrafında yörünge hareketi
        neptuneSelfRotateAngle += 2.5f; //  kendi ekseni etrafında dönüşü


        // yörünge matrisini günceller
        neptuneOrbitMatrix.setToIdentity();
        neptuneOrbitMatrix.rotate(neptuneOrbitAngle, 0.0f, 1.0f, 0.0f); // Yörüngede Y ekseninde döndür
        neptuneOrbitMatrix.translate(-1.0f * 75.0f, 0.0f, 0.0f);

        //  kendi ekseni etrafında dönüş matrisini günceller
        neptuneSelfRotateMatrix.setToIdentity();
        neptuneSelfRotateMatrix.rotate(neptuneSelfRotateAngle, 0.0f, 1.0f, 0.0f); // Kendi etrafında Y ekseninde döndür


        // model matrisini oluşturuluyor ve çiziliyor
        QMatrix4x4 neptuneModelMatrix = neptuneOrbitMatrix * neptuneSelfRotateMatrix * rotateMatrix;
        f->glUniformMatrix4fv(scaleMatrixID,1,GL_FALSE, neptuneScaleMatrix.constData());
        f->glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, neptuneModelMatrix.constData());

        // texture u etkinleştiriliyor
        f->glBindTexture(GL_TEXTURE_2D, neptuneTexture);

        ef->glBindVertexArray(sphereVAO[0]);
        f->glDrawElements(GL_TRIANGLE_STRIP, sunSize, GL_UNSIGNED_INT, 0);
    }

    update();
}

void QOpenGLPanel::resizeGL(int width, int height)
{


}
