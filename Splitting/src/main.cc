// main.cc

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "gl-utils.h"
#include "trackball.h"
#include "image.h"

#include "time.h"

static void init();

GLuint program;        // Programme classique, pour texturer le chat
GLuint program_normal; // Programme avec les trois shaders, pour afficher les normales
unsigned nb_indices;

vec3 middleCat;
float angle = 0.0f;

struct GlobalState
{
  vec2       begin;          // Position de la souris.
  quaternion quat;           // Orientation de l'objet.
  quaternion dquat;          // Quaternion différence.
  float      zoom;           // Angle d'ouverture en degrés.
  int        width, height;  // Taille de la fenêtre.
  int        buttons;        // Boutons de la souris.
  GlobalState() : begin(1, 1), quat(), dquat(), zoom(45), width(800), height(800), buttons(0) {}
};

GlobalState state;

/*---------------------------------------------------------------------------*\
 * display_callback                                                          *
\*---------------------------------------------------------------------------*/
static void display_callback()
{  
  matrix projection, look_at;
  matrix modelview;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); PRINT_OPENGL_ERROR();

  projection.set_perspective(state.zoom, static_cast<float>(state.width) / state.height, 1,1000);

  // les paramètres sont dans l'ordre : position de la caméra, position visée, verticale.
  look_at.set_look_at(vec3(-1,30,103.5), vec3(0,16.9,-3.5), vec3(0,1,0));

  modelview = look_at * state.quat.get_matrix();
// std::cout<<clock()<<std::endl;//WARNING: GESTION DU TEMPS AVEC GLUTTIMERFUNC plutot (cf autre tp)
  // Dessin du chat, normalement, texturé
//   glUseProgram(program);                                                             PRINT_OPENGL_ERROR();
//   glUniform1i(get_uni_loc(program, "tex"), 0);                                       PRINT_OPENGL_ERROR();
//   glUniformMatrix4fv(get_uni_loc(program, "projection"), 1, GL_FALSE, projection.m); PRINT_OPENGL_ERROR();
//   glUniformMatrix4fv(get_uni_loc(program, "modelview"), 1, GL_FALSE, modelview.m);   PRINT_OPENGL_ERROR();
  // Décommentez la ligne suivante si vous voulez voir le chat texturé:
//    glDrawElements(GL_TRIANGLES, nb_indices, GL_UNSIGNED_SHORT, 0);                    PRINT_OPENGL_ERROR();

  // Dessin du chat, avecullingPointRotationc le programme "normal" comportant un shader de géométrie
  // Les données sont les mêmes, seuls les shaders et leurs éventuels paramètres changent
  glUseProgram(program_normal);                                                             PRINT_OPENGL_ERROR();
  glUniform1i(get_uni_loc(program_normal, "tex"), 0);  
  glUniformMatrix4fv(get_uni_loc(program_normal, "projection"), 1, GL_FALSE, projection.m); PRINT_OPENGL_ERROR();
  glUniformMatrix4fv(get_uni_loc(program_normal, "modelview"), 1, GL_FALSE, modelview.m);          PRINT_OPENGL_ERROR();

  glDrawElements(GL_TRIANGLES, nb_indices, GL_UNSIGNED_SHORT, 0);                           PRINT_OPENGL_ERROR();

  glutSwapBuffers();
}

/*---------------------------------------------------------------------------*\
 * mouse_callback                                                            *
\*---------------------------------------------------------------------------*/
static void mouse_callback (int button, int action, int x, int y)
{
  state.begin = vec2(x, y);
  state.dquat = quaternion();

  if (action == GLUT_DOWN)
    state.buttons |= (1 << button);
  else
    state.buttons &= ~(1 << button);

  glutPostRedisplay();
}

/*---------------------------------------------------------------------------*\
 * motion_callback                                                           *
\*---------------------------------------------------------------------------*/
static void motion_callback(int x, int y)
{
  float width = state.width, height = state.height;

  if (state.buttons & (1 << 0))
  { /* drag in progress, simulate trackball */
    state.dquat =trackball(vec2((2.0 * state.begin.x -           width)    / width,
                                (            height - 2.0 * state.begin.y) / height),
                           vec2((            2.0 * x -             width)  / width,
                                (            height -           2.0 * y)   / height ));
    state.quat = state.quat * state.dquat;
  }
  else if (state.buttons & (1 << 2))
  { /* zooming drag */
    state.zoom -= ((y - state.begin.y) / height) * 40.0;
    state.zoom = std::min(std::max(state.zoom, 5.0f), 120.0f);
  }

  state.begin = vec2(x, y);

  glutPostRedisplay();
}

/*---------------------------------------------------------------------------*\
 * init                                                                      *
\*---------------------------------------------------------------------------*/
static void init()
{
  Image  *image;
  GLuint  texName;
  glClearColor(0.7, 0.8, 0.9, 0);   PRINT_OPENGL_ERROR();
  glEnable(GL_DEPTH_TEST);          PRINT_OPENGL_ERROR();

  // Chargement de la texture du chat
  image = image_load_tga("cat.tga");
  if (image)
  {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);                               PRINT_OPENGL_ERROR();
    glGenTextures(1, &texName);                                          PRINT_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, texName);                               PRINT_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);        PRINT_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);        PRINT_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    PRINT_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);    PRINT_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data); PRINT_OPENGL_ERROR();

    delete image;
  }
  else
    abort();

  vec3 min( 10000, 10000, 10000);
  vec3 max(-10000,-10000,-10000);
  
  // Chargement des données du chat
  std::vector<vec3> positions;
  positions.push_back(vec3(0,0,0));
  std::vector<vec3> normals;
  normals.push_back(vec3(0,0,0));
  std::vector<vec2> tex_coords;
  tex_coords.push_back(vec2(0,0));
  std::vector<short> indices;

  std::string data_dir = DATA_DIR;
  data_dir.append( "/" );
  
  std::ifstream file( data_dir + "cat.obj" );
  while (file)
  {
    std::string line, tag;
    getline(file, line);
    std::stringstream ss(line);
    ss >> tag;
    if (tag == "v")
    {
      float x, y, z;
      ss >> x >> y >> z;
      positions.push_back(vec3(x, y, z));
      if (x < min.x) min.x = x;
      if (y < min.y) min.y = y;
      if (z < min.z) min.z = z;
      if (x > max.x) max.x = x;
      if (y > max.y) max.y = y;
      if (z > max.z) max.z = z;
    } else if (tag == "vn")
    {
      float x, y, z;
      ss >> x >> y >> z;
      normals.push_back(vec3(x, y, z));
    } else if (tag == "vt")
    {
      float u, v;
      ss >> u >> v;
      tex_coords.push_back(vec2(u, v));
    } else if (tag == "f")
    {
      short index;
      ss >> index;
      indices.push_back(index);
      ss >> index;
      indices.push_back(index);
      ss >> index;
      indices.push_back(index);
    }
  }
  
  middleCat = vec3( (min.x + max.x) / 2.0, (min.y + max.y) / 2.0, (min.z + max.z) / 2.0);
  
  // Création et remplissage des vbos
  GLuint vbos[3], vboi, vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(3, vbos);                                                                           PRINT_OPENGL_ERROR();
  glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);                                                          PRINT_OPENGL_ERROR();
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &positions[0], GL_STATIC_DRAW);   PRINT_OPENGL_ERROR();
  glEnableVertexAttribArray(0);                                                                    PRINT_OPENGL_ERROR();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);                                           PRINT_OPENGL_ERROR();

  glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);                                                          PRINT_OPENGL_ERROR();
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);       PRINT_OPENGL_ERROR();
  glEnableVertexAttribArray(1);                                                                    PRINT_OPENGL_ERROR();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);                                           PRINT_OPENGL_ERROR();

  glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);                                                          PRINT_OPENGL_ERROR();
  glBufferData(GL_ARRAY_BUFFER, tex_coords.size() * sizeof(vec2), &tex_coords[0], GL_STATIC_DRAW); PRINT_OPENGL_ERROR();
  glEnableVertexAttribArray(2);                                                                    PRINT_OPENGL_ERROR();
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);                                           PRINT_OPENGL_ERROR();

  glGenBuffers(1, &vboi);                                                                          PRINT_OPENGL_ERROR();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboi);                                                     PRINT_OPENGL_ERROR();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(short), &indices[0], GL_STATIC_DRAW); PRINT_OPENGL_ERROR();
  nb_indices = indices.size();
}

/*---------------------------------------------------------------------------*\
 * reshape_callback                                                          *
\*---------------------------------------------------------------------------*/
static void reshape_callback(int width, int height)
{
  state.width  = width;
  state.height = height;

  glViewport(0,0, width, height); PRINT_OPENGL_ERROR();
  glutPostRedisplay();
}

/*---------------------------------------------------------------------------*\
 * keyboard_callback                                                         *
\*---------------------------------------------------------------------------*/
static void keyboard_callback(unsigned char key, int, int)
{
  switch (key)
  {
    case 'q':
    case 'Q':
    case 27:
      exit(0);
  }
}

/*****************************************************************************\
 * special_callback                                                          *
\*****************************************************************************/
static void special_callback(int key, int,int)
{
  switch (key)
  {
    case GLUT_KEY_UP:
      angle += 1.0f;
      break;
    case GLUT_KEY_DOWN:
      angle -= 1.0f;
      break;
  }
  glutPostRedisplay();
}

/*****************************************************************************\
 * main                                                                      *
\*****************************************************************************/
int main(int argc, char *argv[])
{
  glutInitContextVersion(3, 3); 
  //glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
  //glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(state.width, state.height);
  glutCreateWindow("Debug Normal");
  glutDisplayFunc(display_callback);
  glutMotionFunc(motion_callback);
  glutMouseFunc(mouse_callback);
  glutReshapeFunc(reshape_callback);
  glutKeyboardFunc(keyboard_callback);
  glutSpecialFunc(special_callback);

  glewExperimental = true; glewInit(); glGetError();
  init();

  // Création du programme "texture"
  program = read_shader("shader.vert", "shader.frag",{"position","normal","tex_coord"});
  // Création du programme "normale"
  program_normal = read_shader("normal.vert", "normal.geom", "normal.frag",
                               {"position","normal","tex_coord"});

  glutMainLoop();

  return 0;
}

