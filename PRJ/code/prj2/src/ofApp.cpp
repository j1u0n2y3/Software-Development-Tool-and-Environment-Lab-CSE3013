#include "ofApp.h"

// #define DEBUG_LOG /* Define if you want to see debug logging messages. */

#define MAX_POINTS 20
unsigned int clicked = 0;

int dx[4] = {0, 0, -1, 1};
int dy[4] = {-1, 1, 0, 0};
queue<pair<int, int>> q;

//--------------------------------------------------------------
void ofApp::setup()
{
    /* INIT - variables */
    bAddingPointsMode = true;
    BFSMode = false;
    BFSX = BFSY = -1;
    dstX = dstY = -1;
    perc.load("perc.wav");
    notify.load("notify.wav");
    loop.load("loop.mp3");
    loop.setLoop(true);
    score.load("score.wav");
    score.setLoop(true);
    score.setVolume(0.6);
    coin.load("coin.wav");
    loop.play();
    set_field();

    /* INIT - window */
    ofSetFrameRate(60);
    ofSetWindowShape(windowWidth, windowHeight);
    ofSetWindowTitle("20211584 Junyeong JANG");
    ofSetWindowPosition((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2);
    ofBackground(255, 255, 255);
}

//--------------------------------------------------------------
unsigned long long int timestamp = 0;
void ofApp::update()
{
    timestamp = (timestamp + 1) % ((unsigned long long int)0xFFFFFFFFFFFFFFFF);

    ofSetWindowShape(windowWidth, windowHeight);
    if (bAddingPointsMode)
    {
        float elapsed = ofGetElapsedTimef();
        int greyscale = (int)((sin(elapsed * 2) * 0.5 + 0.5) * 70 + 90);
        ofBackground(greyscale);
    }
    else
        ofBackground(255, 255, 255);

    if (bAddingPointsMode)
        loop.setVolume(0.25);
    else
        loop.setVolume(1.0);

#ifdef DEBUG_LOG
    if (!(timestamp % 120))
    {
        print_field();
        printf("\n");
    }
#endif
    /*** BFS ***/
    /* Loop through max(q.size(), 100) */
    for (int i = 0; i <= 80 && !q.empty(); i++)
    {
        /* Retrieves the current position from the queue. */
        int x = q.front().second;
        int y = q.front().first;
        q.pop();

        for (int i = 0; i < 4; i++)
        {
            /* Move up, down, left, and right. */
            int nx = x + dx[i];
            int ny = y + dy[i];

            /* Check if the position is movable within the field's range. */
            if (nx >= 0 && nx < 120 && ny >= 0 && ny < 90)
            {
                if (field[ny][nx] == '~') /* not visited */
                {
                    /* Enqueue the movable position and mark it with '*'(visited). */
                    q.push({ny, nx});
                    field[ny][nx] = '*';
                    /* Store the previous coordinates moved from. */
                    prev[ny][nx] = {x, y};
                    /* If the destination is reached,
                     * clear the queue and find the shortest path.
                     */
                    if (nx == dstX && ny == dstY)
                    {
                        q = queue<pair<int, int>>();
#ifdef DEBUG_LOG
                        printf("[ALERT] traceBack start\n");
#endif
                        traceBack(dstX, dstY);
#ifdef DEBUG_LOG
                        printf("[ALERT] traceBack end\n");
#endif
                        score.stop();
                        coin.play();
                        return;
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofTrueTypeFont font;
    font.load("verdana.ttf", 12);

    /*** BFS FIELD ***/
    if (BFSMode)
    {
        int blockSize = 10;
        /* FIELD BLOCK */
        /* Loop through the entire size of the field */
        for (int y = 0; y < 90; ++y)
        {
            for (int x = 0; x < 120; ++x)
            {
                if (field[y][x] == '*') /* visited */
                {
                    float distance = abs(x - BFSX) + abs(y - BFSY); /* distance from the starting point */
                    float ratio = distance / 120;                   /* ratio based on distance */

                    /* Color calculation: from yellow to blue */
                    int red = min(255 * (1 - ratio), (float)255);
                    int green = min(255 * (1 - ratio), (float)255);
                    int blue = min(255 * ratio, (float)255);

                    ofSetColor(red, green, blue);
                    ofDrawRectangle(x * blockSize, y * blockSize, blockSize, blockSize);
                }
                else if (field[y][x] == 'O') /* shortest path */
                {
                    ofSetColor(ofColor::red);
                    ofDrawRectangle(x * blockSize, y * blockSize, blockSize, blockSize);
                }
            }
        }
        /* GRID POINT */
        ofSetColor(ofColor::black);
        /* Loop through the entire size of the field */
        for (int y = 0; y <= 90; y++)
            for (int x = 0; x <= 120; x++)
                ofDrawRectangle(x * blockSize, y * blockSize, 1, 1);
    }

    /*** POLYGONS ***/
    for (auto &polygon : polygons)
    {
        /* FILL */
        ofFill();
        ofSetColor(polygon.r, polygon.g, polygon.b);
        ofBeginShape();
        for (auto &vertex : polygon.convexhull)
            ofVertex(vertex.x, vertex.y);
        ofEndShape(true);

        /* EDGES */
        ofNoFill();
        ofSetColor(0, 0, 0);
        ofBeginShape();
        for (auto &vertex : polygon.convexhull)
            ofVertex(vertex.x, vertex.y);
        ofEndShape(true);

        /* AREA(CENTER) */
        POINT2D center = {0, 0};
        /* Calculate center point as the average of all points. */
        for (auto &vertex : polygon.convexhull)
        {
            center.x += vertex.x;
            center.y += vertex.y;
        }
        center.x /= polygon.convexhull.size();
        center.y /= polygon.convexhull.size();

        /* Display polygon area at the center. */
        string areaStr = ofToString(polygon.area);
        ofSetColor(0, 0, 0);
        font.drawString(areaStr, center.x, center.y);
    }

    /*** CURRENT POINTS ***/
    ofFill();
    ofSetColor(0, 0, 0);
    for (auto &point : currentPoints)
        ofDrawCircle(point.x, point.y, 3);

    /*** DST POINT ***/
    ofSetColor(ofColor::red);
    if (dstX != -1 && dstY != -1)
        ofDrawCircle(dstX * 10 + 5, dstY * 10 + 5, 5);
}

//--------------------------------------------------------------
void ofApp::exit()
{
    printf("\n\nThere is no memory in the heap area grabbed by commands like 'new', 'malloc'.\n");
    printf("In addition, when all used containers(vector, queue, stack, ...) are no longer in use, ");
    printf("destructors are called to clean up the memory area.\n");
    printf("Therefore, no additional memory cleanup routines are required.\n\n");
    printf("The program ends after 5 seconds.\n");
    Sleep(5000);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == ' ') /* SPACE */
    {
        bAddingPointsMode = !bAddingPointsMode;
        if (bAddingPointsMode)
            printf("[MODE] Adding Dot\n");
        else
            printf("[MODE] Generating Field\n");

        if (!bAddingPointsMode && currentPoints.size() > 2)
        {
            /* Create a new convex hull from currentPoints. */
            vector<POINT2D> mergedPoints = convexHull(currentPoints);

            /* Loop through all polygons */
            for (auto it = polygons.begin(); it != polygons.end();)
            {
                /* If the newly created polygon(mergedPoints) overlaps an existing polygon, */
                if (overlaps(it->convexhull, mergedPoints))
                {
                    /* Merge two polygons' points. */
                    mergedPoints.insert(mergedPoints.end(), it->convexhull.begin(), it->convexhull.end());
                    mergedPoints = convexHull(mergedPoints);
                    /* Delete the overlapping polygon and update the iterator. */
                    it = polygons.erase(it);
                }
                /* Otherwise, */
                else
                    /* Move to the next polygon. */
                    ++it;
            }

            /* Create a new polygon from the merged points. */
            POLYGON newPolygon;
            newPolygon.convexhull = mergedPoints;
            newPolygon.set_convex_hull();
            polygons.push_back(newPolygon);
            /* Set a new field. */
            set_field();
            notify.play();
            clicked = 0;
#ifdef DEBUG_LOG
            print_field();
#endif
            /* Clear the current points for the next polygon. */
            currentPoints.clear();
        }
    }
    if (key == 'r' || key == 'R') /* R */
    {
        /* Initialize the field and the points. */
        currentPoints = vector<POINT2D>();
        polygons = vector<POLYGON>();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
    if (button == OF_MOUSE_BUTTON_LEFT) /* LEFT */
    {
        if (bAddingPointsMode && clicked < MAX_POINTS)
        {
            /* Add a point. */
            currentPoints.push_back(POINT2D(x, y));
            clicked++;
            perc.play();
        }
    }
    else if (button == OF_MOUSE_BUTTON_RIGHT) /* RIGHT */
    {
        prev_bAddingPointsMode = bAddingPointsMode;
        bAddingPointsMode = false;
        printf("[BFS] Start\n");
        /* Prepare for BFS. */
        BFSX = x / 10;
        BFSY = y / 10;
        BFSMode = true;
        if (field[BFSY][BFSX] != '#' && !(BFSX == dstX && BFSY == dstY))
        {
            field[BFSY][BFSX] = '*';
            prev[BFSY][BFSX] = {-1, -1};
            q.push({BFSY, BFSX});
            score.play();
        }
    }
    else if (button == OF_MOUSE_BUTTON_2) /* WHEEL */
    {
        /* Set the destination point. */
        dstX = x / 10;
        dstY = y / 10;
        printf("[BFS] dstX : %d / dstY : %d\n", dstX, dstY);
        perc.play();
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
    if (button == OF_MOUSE_BUTTON_RIGHT) /* RIGHT */
    {
        printf("[BFS] End\n");
        /* Stop BFS. */
        BFSMode = false;
        q = queue<pair<int, int>>();
        set_field();
        bAddingPointsMode = prev_bAddingPointsMode;
        score.stop();
    }
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
}

void ofApp::set_field()
{
    /* Loop through every block in field */
    for (int y = 0; y < 90; y++)
    {
        for (int x = 0; x < 120; x++)
        {
            bool flag = false;
            /* Loop through all polygons */
            for (const auto &polygon : polygons)
            {
                /* If a polygon exists in that block, */
                if (isPolygonInBlock(polygon, x, y))
                {
                    /* Set field blocked. */
                    field[y][x] = '#';
                    flag = true;
                    break;
                }
            }
            /* Otherwise, */
            if (!flag)
                /* Set field not blocked. */
                field[y][x] = '~';
        }
    }
#ifdef DEBUG_LOG
    printf("[ALERT] set_field complete\n");
#endif
}

void ofApp::print_field()
{
    for (int y = 0; y < 90; y++)
    {
        for (int x = 0; x < 120; x++)
        {
            printf("%c", field[y][x]);
        }
        printf("\n");
    }
}

void ofApp::traceBack(int dstX, int dstY)
{
    /* Initialize x and y with the destination coordinates. */
    int x = dstX, y = dstY;
    /* Loop until the previous node is the starting node */
    while (0 <= x && x < 120 && 0 <= y && y < 90 &&
           prev[y][x].first != -1)
    {
        /* Mark the current location as part of the shortest path. */
        field[y][x] = 'O';
        /* Move to the previous location in the path. */
        x = prev[y][x].first;
        y = prev[y][x].second;
    }
    /* Mark the starting location in the shortest path. */
    field[BFSY][BFSX] = 'O';
}