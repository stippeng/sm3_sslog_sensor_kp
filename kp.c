
/* 
 * Author: Stefan Grönroos <dev@stefang.net>
 * An attempt at a simple library for inserting sensor values into                 
 * Smart-M3 using the C KPI. 
 * Based on HelloWorld example by Aleksandr A. Lomov 
 * in the Smart-M3 C KPI Library.
 *  
 * Copyright (c) 2016, Stefan Grönroos.			
 * Copyright (c) 2016, Åbo Akademi University.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "ssn.h"
#include <uuid.h>
#include <time.h>

#define XML_DATE "^^<http://www.w3.org/2001/XMLSchema#dateTime>"


typedef struct {
  uuid_t id;
  char str[37];
} uniq_t;

static char *gen_uuid(uniq_t *u)
{
  uint32_t status;
  char *s;
  uuid_create(&u->id, &status);
  uuid_to_string(&u->id, &s, &status);
  strncpy(u->str, s, 37);
  free(s);

  return u->str;
}

void insert_float_obs(float val, time_t t) {
    individual_t *obs = sslog_new_individual(CLASS_OBSERVATION);
    individual_t *s_obs = sslog_new_individual(CLASS_SENSOROUTPUT);
    individual_t *obs_val = sslog_new_individual(CLASS_OBSERVATIONVALUE);
    individual_t *obs_t = sslog_new_individual(CLASS_TIMEINTERVAL);
    individual_t *dummysens = sslog_new_individual(CLASS_SENSOR);

    uniq_t uuid;
    
    char s_val[16];
    snprintf(s_val, sizeof(s_val), "%8.4f", val);

    char s_time[128];
    strftime(s_time, sizeof(s_time), "%FT%TZ", gmtime(&t));
    strlcat(s_time, XML_DATE, sizeof(s_time));
    fprintf(stderr, "Time: %s\n", s_time);

    sslog_set_individual_uuid(obs, gen_uuid(&uuid));
    sslog_set_individual_uuid(s_obs, gen_uuid(&uuid));
    sslog_set_individual_uuid(obs_val, gen_uuid(&uuid));
    sslog_set_individual_uuid(obs_t, gen_uuid(&uuid));
    sslog_set_individual_uuid(dummysens, gen_uuid(&uuid));

    sslog_add_property(obs, PROPERTY_OBSERVEDBY, dummysens);
    sslog_add_property(obs, PROPERTY_OBSERVATIONRESULT, s_obs);
    
    sslog_add_property(s_obs, PROPERTY_HASVALUE, obs_val);
    sslog_add_property(obs_val, PROPERTY_HASREGIONDATAVALUE, s_val);

    sslog_add_property(obs, PROPERTY_OBSERVATIONRESULTTIME, obs_t);
    sslog_add_property(obs_t, PROPERTY_HASREGIONDATAVALUE, s_time);
    
    
    sslog_ss_insert_individual(obs);
    sslog_ss_insert_individual(s_obs);
    sslog_ss_insert_individual(obs_val);
    sslog_ss_insert_individual(obs_t);
}

int main()
{
sslog_ss_init_session_with_parameters("X", "130.232.87.18", 10020);
    register_ontology();
    
    if (ss_join(sslog_get_ss_info(), "KP_1") == -1) {
        printf("Can't join to SS\n");
        return 0;
    }

    printf("\nKP joined SS");

// Set your code.
    time_t now;
    time(&now);
    insert_float_obs(13.0f, now);



    sslog_repo_clean_all();
    sslog_ss_leave_session(sslog_get_ss_info());
    
    printf("\nKP leave SS...\n");

    return 0;
}

