#/usr/bin python3

import http.client
import urllib
import urllib.request

header = {  'Host':'doc-server',
            'Accept-Language':'zh-CN',
            'Connection':'Keep-Alive',
            'Accept':'image/png, image/svg+xml, image/jxr, image/*;q=0.8, */*;q=0.5',
            'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.135 Safari/537.36 Edge/12.10240',
            'Accept-Encoding':'gzip, deflate'
}

user = ''

def test_login(connection):
    connection.request(method='GET', url='/login.asp', headers=header)
    response = connection.getresponse()
    data = response.read().decode('utf-8')
    index = data.find('userName')
    global user 
    user = data[index:index+11]
    cookies = response.getheader('Set-Cookie')
    if cookies != None:
        cookie = cookies.split(';')[0]
        header['Cookie'] = cookie

def test_check(connection):
    header['Content-Type'] = 'application/x-www-form-urlencoded'
    header['Referer'] = 'http://doc-server/login.asp'
    body = 'RedirectURL=/Default.asp&push_type=2&'+user+'=wml1951&password=ppDemo22'
    connection.request(method='POST', url='/confirm.asp', body=body, headers=header)
    response = connection.getresponse()
    data = response.read()
    location_url = response.getheader('Location')
    connection.request(method='GET', url=location_url, headers=header)
    response = connection.getresponse()
    data = response.read().decode('utf-8')
    index = data.find('font color="#FF0000" style="font-size: 12pt"') #get check from this tag
    check_time = data[index+53:index+72]
    print(check_time)

def do_check():
    connection = http.client.HTTPConnection('doc-server')
    test_login(connection)
    test_check(connection)
    connection.close()

if __name__ == '__main__':
    do_check()
    input()

