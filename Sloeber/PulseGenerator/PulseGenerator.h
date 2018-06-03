/*
 * Copyright 2018 Cheolmin Jo (webos21@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef PULSEGENERATOR_H
#define PULSEGENERATOR_H

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#define DPIN_RPM       6
#define DPIN_SPEED     7

typedef struct s_main_t {
	bool    run_rpm;
	bool    run_speed;
	bool    on_rpm;
	bool    on_speed;
	int64_t last_rpm;
	int64_t last_speed;
} main_t;

void gen_start_rpm(void);
void gen_stop_rpm(void);

void gen_start_speed(void);
void gen_stop_speed(void);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* PULSEGENERATOR_H */
