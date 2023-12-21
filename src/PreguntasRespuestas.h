#ifndef  PREGUNTAS_RESPUESTAS_H
#define PREGUNTAS_RESPUESTAS_H
#include <Arduino.h>

struct PreguntasRespuesta {
    int pregunta;
    int tipoPregunta; //el tipo de pregunta determinara el tipo de respuesta que se espera, 1 => palabras que inicien con, 2 => parabras que termninen con, 3 => cantidad de silabas
    String respuesta;
};

extern const int tamano;
extern PreguntasRespuesta preguntas[];

#endif