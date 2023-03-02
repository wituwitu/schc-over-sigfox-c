#include "ack.h"
#include "casting.h"
#include "misc.h"

void ack_to_bin(CompoundACK *ack, char dest[DOWNLINK_MTU_BITS + 1]) {
  memset(dest, '\0', DOWNLINK_MTU_BITS + 1);
  char *message = ack->message;
  bytes_to_bin(dest, message, sizeof(message));
}

void init_rule_from_ack(Rule *dest, CompoundACK *ack) {
  char as_bin[DOWNLINK_MTU_BITS + 1];
  ack_to_bin(ack, as_bin);
  init_rule(dest, as_bin);
}

void get_ack_rule_id(Rule *rule, CompoundACK *ack,
                     char dest[rule->rule_id_size + 1]) {
  int rule_id_index = 0;
  int rule_id_size = rule->rule_id_size;
  char ack_as_bin[DOWNLINK_MTU_BITS + 1];
  ack_to_bin(ack, ack_as_bin);
  strncpy(dest, ack_as_bin + rule_id_index, rule_id_size);
  dest[rule_id_size] = '\0';
}

void get_ack_dtag(Rule *rule, CompoundACK *ack, char dest[rule->t + 1]) {
  int dtag_index = rule->rule_id_size;
  int dtag_size = rule->t;
  char ack_as_bin[DOWNLINK_MTU_BITS + 1];
  ack_to_bin(ack, ack_as_bin);
  strncpy(dest, ack_as_bin + dtag_index, dtag_size);
  dest[dtag_size] = '\0';
}

void get_ack_w(Rule *rule, CompoundACK *ack, char dest[rule->m + 1]) {
  int w_index = rule->rule_id_size + rule->t;
  int w_size = rule->m;
  char ack_as_bin[UPLINK_MTU_BITS + 1];
  ack_to_bin(ack, ack_as_bin);
  strncpy(dest, ack_as_bin + w_index, w_size);
  dest[w_size] = '\0';
}

void get_ack_c(Rule *rule, CompoundACK *ack, char dest[2]) {
  int c_index = rule->rule_id_size + rule->t + rule->m;
  char ack_as_bin[DOWNLINK_MTU_BITS + 1];
  ack_to_bin(ack, ack_as_bin);
  strncpy(dest, ack_as_bin + c_index, 1);
  dest[1] = '\0';
}

void get_ack_bitmap(Rule *rule, CompoundACK *ack,
                    char dest[rule->window_size + 1]) {
  int bitmap_index = rule->rule_id_size + rule->t + rule->m + 1;
  char ack_as_bin[DOWNLINK_MTU_BITS + 1];
  ack_to_bin(ack, ack_as_bin);
  strncpy(dest, ack_as_bin + bitmap_index, rule->window_size);
  dest[rule->window_size] = '\0';
}

int get_ack_nb_tuples(Rule *rule, CompoundACK *ack) {
  char as_bin[DOWNLINK_MTU_BITS + 1];
  ack_to_bin(ack, as_bin);
  char window[rule->m + 1];
  char first_window[rule->m + 1];
  memset(window, '\0', rule->m + 1);
  memset(first_window, '0', rule->m);
  first_window[rule->m] = '\0';
  int tuple_index = rule->ack_header_length + rule->window_size;
  char *p = as_bin + tuple_index;

  int nb_tuples = 1;
  strncpy(window, p, rule->m);
  while (strcmp(window, first_window) != 0 &&
         p < (as_bin + DOWNLINK_MTU_BITS - tuple_index)) {
    nb_tuples++;
    p += rule->m + rule->window_size;
    strncpy(window, p, rule->m);
  }

  return nb_tuples;
}

void get_ack_tuples(Rule *rule, CompoundACK *ack, int nb_tuples,
                    char windows[nb_tuples][rule->m + 1],
                    char bitmaps[nb_tuples][rule->window_size + 1]) {

  char as_bin[DOWNLINK_MTU_BITS + 1];
  ack_to_bin(ack, as_bin);

  char window[rule->m + 1];
  char bitmap[rule->window_size + 1];
  get_ack_w(rule, ack, window);
  get_ack_bitmap(rule, ack, bitmap);
  strncpy(windows[0], window, rule->m + 1);
  strncpy(bitmaps[0], bitmap, rule->window_size + 1);

  int tuple_index = rule->ack_header_length + rule->window_size;
  char *p = as_bin + tuple_index;

  for (int i = 1; i <= nb_tuples; i++) {
    strncpy(windows[i], p, rule->m);
    strncpy(bitmaps[i], p + rule->m, rule->window_size);
    windows[i][rule->m] = '\0';
    bitmaps[i][rule->window_size] = '\0';
    p += rule->m + rule->window_size;
  }
}

int is_ack_receiver_abort(Rule *rule, CompoundACK *ack) {
  char w[rule->m + 1], c[2];
  get_ack_w(rule, ack, w);
  get_ack_c(rule, ack, c);

  if (is_monochar(w, '1') && strcmp(c, "1") == 0) {
    char as_bin[DOWNLINK_MTU_BITS + 1];
    ack_to_bin(ack, as_bin);

    int header_length = rule->ack_header_length;
    char *padding_i = as_bin + header_length;
    char *padding_f = strrchr(as_bin, '1') + 1;
    if (padding_f <= padding_i) return 0;

    int padding_size = (int) (padding_f - padding_i);
    char padding[padding_size + 1];
    strncpy(padding, as_bin + header_length, padding_size);
    padding[padding_size] = '\0';

    int padding_start_size = padding_size - L2_WORD_SIZE;
    char padding_start[padding_start_size + 1];
    char padding_end[L2_WORD_SIZE + 1];
    strncpy(padding_start, padding, padding_start_size);
    strncpy(padding_end, padding + padding_start_size, L2_WORD_SIZE);
    padding_start[padding_start_size] = '\0';
    padding_end[L2_WORD_SIZE] = '\0';

    if (is_monochar(padding_end, '1') && strlen(padding_end) == L2_WORD_SIZE) {
      if (strcmp(padding_start, "") != 0 && header_length % L2_WORD_SIZE != 0) {
        return is_monochar(padding_start, '1') && (header_length + padding_start_size) % L2_WORD_SIZE == 0;
      }
      return header_length % L2_WORD_SIZE == 0;
    }
  }
  return 0;
}

int is_ack_compound(Rule *rule, CompoundACK *ack) {
  char as_bin[DOWNLINK_MTU_BITS + 1];
  ack_to_bin(ack, as_bin);
  char *padding = as_bin + rule->ack_header_length;

  return !is_ack_receiver_abort(rule, ack) && !is_monochar(padding, '0');
}

int is_ack_complete(Rule *rule, CompoundACK *ack) {
  char c[2];
  get_ack_c(rule, ack, c);

  return !is_ack_receiver_abort(rule, ack) && strcmp(c, "1") == 0;
}

void generate_receiver_abort(Rule *rule, Fragment *src, CompoundACK *dest) {
  char rule_id[rule->rule_id_size + 1];
  char dtag[rule->t + 1];
  char w[rule->m + 1];
  char c[] = "1";

  get_fragment_rule_id(rule, src, rule_id);
  get_fragment_dtag(rule, src, dtag);
  memset(w, '1', rule->m);

  int dtag_idx = rule->rule_id_size;
  int w_idx = dtag_idx + rule->t;
  int c_idx = w_idx + rule->m;
  int padding_idx = c_idx + 1;

  int header_length = rule->ack_header_length;
  int header_remainder = header_length % L2_WORD_SIZE;

  int padding_size = header_remainder == 0
                         ? L2_WORD_SIZE
                         : 2 * L2_WORD_SIZE - header_remainder;

  char padding[padding_size + 1];
  memset(padding, '1', padding_size);
  padding[padding_size] = '\0';

  char ra_bin[DOWNLINK_MTU_BITS + 1];
  memset(ra_bin, '0', DOWNLINK_MTU_BITS);
  ra_bin[DOWNLINK_MTU_BITS] = '\0';

  strncpy(ra_bin, rule_id, rule->rule_id_size);
  strncpy(ra_bin + dtag_idx, dtag, rule->t);
  strncpy(ra_bin + w_idx, w, rule->m);
  strncpy(ra_bin + c_idx, c, 1);
  strncpy(ra_bin + padding_idx, padding, padding_size);

  memset(dest, '\0', sizeof(CompoundACK));
  bin_to_bytes(dest->message, ra_bin, DOWNLINK_MTU_BITS);
}