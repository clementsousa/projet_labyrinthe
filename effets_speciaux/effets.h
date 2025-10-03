#ifndef __EFFETS_H__
#define __EFFETS_H__

float complex agrandissement(float complex z, float zoom);
float complex rotation(float complex centre_de_rotation, float tetea);
float complex rotation_inverse(float complex centre_de_rotation, float teta);
float complex translation(float complex origine, float complex deplacement);
float complex translation(float complex origine, float complex deplacement);
float complex rotation_changement_de_centre(float complex z, float complex nouvelle_origine, float teta);
float complex transformation_rotation_centre(float complex z,float complex nouvelle_origine, float teta, float rayon);
float complex loupe(float complex z, float complex position, float rayon, float force);
float complex rotation_inverse_cercle(float complex z, float complex nouvelle_origine, float teta, float rayon);

#endif