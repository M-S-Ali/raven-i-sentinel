#ifndef BQ76925_H
#define BQ76925_H

#define voltage_pin A0
#define current_pin A1
#define temperature_pin A2

class LFP_Battery {
  public:
    int cell_v[6];
    int pack_v;
    int pack_i;
    int pack_temperature;
    uint8_t cell_bal[6];

    void init();
    void update_cell_v();
    void update_pack_param();
    void set_balancing(uint8_t cell);
    void reset_balancing(uint8_t cell);
    void update_bal_status();
    
  private:
    const int bal_cell_v = 340;
    int8_t vref_gc;
    int8_t vref_oc;
    int8_t vcn_oc[6];
    int8_t vcn_gc[6];

    void calibrate();
};

#endif