import json
import glob
import struct

if __name__ == '__main__':
    binary = bytearray()
    g = glob.glob('stores_full_json/*.json')
    n_stores = len(g)
    print(f'{n_stores} stores total')
    for name in g:
        with open(name, 'r') as f:
            store = json.loads(f.read())
            # we want lat as double, lon as double, store address as
            # string, and store location number as a unique identifier
            store_latitude = float(store['latitude'])
            store_longitude = float(store['longitude'])
            store_location_number = int(store['locationNumber'])
            store_address = store['address']
            # assemble the address into basically the normal postal address format
            store_address_str = store_address['street'] + ' ' + store_address['city'] + ', ' + store_address['state'] + ' ' + store_address['zip']

            # data format (all little endian):
            # 4 bytes - number of stores
            # (then data per store, repeats)
            # 4 bytes - location number
            # 4 bytes - latitude (float32)
            # 4 bytes - longitude (float32)
            # 128 bytes - null-terminated street address

            
            binary.extend(
                struct.pack('<Iff128s',
                            store_location_number,
                            store_latitude,
                            store_longitude,
                            bytes(store_address_str, 'utf-8')))

    

    # can't insert a bytes object, but you can insert its individual bytes
    n = struct.pack('<I', n_stores)
    binary.insert(0, n[3])
    binary.insert(0, n[2])
    binary.insert(0, n[1])
    binary.insert(0, n[0])
    
    f = open('stores.bin', 'wb')
    f.write(binary)
    f.close()
