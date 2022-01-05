let VE = {
    _create: ffi('void* mgos_vedirect_create()'),    
    _read: ffi('char* mgos_vedirect_read()'),
    _print: ffi('void mgos_vedirect_print()'),
    _destroy: ffi('void mgos_vedirect_destroy()'),

    create: function(i2cAddress) {
        let obj = Object.create(VE._proto);
        obj.ve = VE._create(i2cAddress);
        return obj;
    },

    _proto: {
      destroy: function() {
        return VE._close(this.ads);
      },

      read: function() {
        let rd = VE._read();
        Log.info(rd)
        return JSON.parse(rd);
      }
    }
};