import glob
import json


if __name__ == '__main__':
    longest_address = ''
    
    for name in glob.glob('stores_full_json/*'):
        with open(name, 'r') as f:
            store = json.loads(f.read())
            store_address = store['address']
            store_address_str = store_address['street'] + ' ' + store_address['city'] + ', ' + store_address['state'] + ' ' + store_address['zip']
            if len(store_address_str) > len(longest_address):
                longest_address = store_address_str

    print(f'longest address: {longest_address}')
    print(f'len {len(longest_address)}')
            
