import React, { useEffect, useState } from 'react';
import { View, Text, TextInput, Button, ActivityIndicator, StyleSheet, TouchableOpacity } from 'react-native';
import { Audio } from 'expo-av';


const App = () => {
  // State variables.
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);
  const [foodQuantity, setFoodQuantity] = useState(null); // State for food quantity input.
  const [message, setMessage] = useState(null); // State for API response message.

  // Funktion to play blllooop sound.
  const playEggSound = async () => {
    try {
      const { sound } = await Audio.Sound.createAsync(
        require('./assets/egg_sound.mp3') 
      );
      await sound.playAsync();
    } catch (error) {
      console.error('Fejl ved afspilning af lyd:', error);
    }
  };

  // Fetch data from HENS API.
  const fetchData = async () => {
    try {
      const response = await fetch('https://hensstatus.com:8080/nest_status?UUID=4569b4bb-eaf4-451d-b054-37f359a6d6b');
      const json = await response.json();
      setData(json.data);

      // Check egg_just_now from API and pllay sounnd if true.
      if (json.data.egg_just_now) {
        playEggSound();
      }

    } catch (error) {
      console.error('Uventet fejl:', error);
    } finally {
      setLoading(false);
    }
  };

  // Fetch data on load and every 1/2 seconds.
  useEffect(() => {
    const interval = setInterval(fetchData, 500);
    return () => clearInterval(interval);
  }, []);

  const setFoodQuantityAPI = async () => {
    try {
      const url = 'https://hensstatus.com:8080/set_food?quantity=' + foodQuantity + '&UUID=4569b4bb-eaf4-451d-b054-37f359a6d6b';
      const response = await fetch(url);
      const json = await response.json();
      
      // Set the message with success or error response.
      if (json.status="success") {
        setMessage("Foder opdateret ");
      } else {
        setMessage("Kunne ikke opdatere foder.");
      }

    } catch (error) {
      setMessage("Der opstod en uventet fejl ved opdatering af foder.");
    }
  };
  
  // We are loading.
  if (loading) {
    return <ActivityIndicator />;
  }

  // Loading failed.
  if (!data) {
    return <Text>Der skete en uventet fejl.</Text>;
  }

  // Render JSON data.
  return (
    <View style={styles.container}>
      <Text style={styles.headerText}>🐔 Rede overblik 🐔</Text>

      <View style={styles.card}>
        <Text style={styles.label}>🥚 Æg idag</Text>
        <Text style={styles.value}>{data.egg_count}</Text>
      </View>
      <View style={styles.card}>
        <Text style={styles.label}>🌾 Fodring idag</Text>
        <Text style={styles.value}>{data.food_quantity} gram</Text>
      </View>
      <View style={styles.card}>
        <Text style={styles.label}>💡 Lys</Text>
        <Text style={styles.value}>{data.light_status ? 'Ja' : 'Nej'}</Text>
      </View>
      <View style={styles.card}>
        <Text style={styles.label}>🐔 Sidste læggetid</Text>
        <Text style={styles.value}>{data.lay_time} sekunder.</Text>
      </View>
      <View style={styles.card}>
        <Text style={styles.label}>🥚 Sidste ægtid</Text>
        <Text style={styles.value}>{data.last_egg_datetime}</Text>
      </View>

      {/* Input for setting food quantity */}
      <TextInput
        style={styles.input}
        placeholder="Indtast foder mængde"
        value={foodQuantity}
        onChangeText={setFoodQuantity}
        keyboardType="numeric"
      />

      <TouchableOpacity style={styles.button} onPress={setFoodQuantityAPI}>
        <Text style={styles.buttonText}>Opdater foder mængde</Text>
      </TouchableOpacity>

      {/* Message after API call */}
      {message && <Text style={styles.message}>{message}</Text>}
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#ffeaea',
    alignItems: 'center',
    padding: 20,
  },
  headerText: {
    fontSize: 32,
    fontWeight: 'bold',
    color: '#000000',
    marginBottom: 20,
    textAlign: 'center',
  },
  card: {
    flexDirection: 'row',
    alignItems: 'center',
    backgroundColor: '#fff3f3',
    padding: 15,
    marginVertical: 5,
    width: '100%',
    borderRadius: 10,
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.2,
    shadowRadius: 5,
  },
  label: {
    fontSize: 24,
    color: '#ff6b6b',
    fontWeight: 'bold',
    flex: 1,
  },
  value: {
    fontSize: 24,
    color: '#333',
  },
  input: {
    width: '100%',
    padding: 10,
    fontSize: 32,
    borderColor: '#ff6b6b',
    borderWidth: 1,
    borderRadius: 5,
    marginVertical: 10,
  },
  message: {
    marginTop: 10,
    fontSize: 14,
    color: '#ff6b6b',
  },
  button: {
    backgroundColor: '#ff6b6b',
    padding: 15,
    borderRadius: 5,
    alignItems: 'center',
    marginVertical: 10,
  },
  buttonText: {
    color: '#fff',
    fontSize: 24,
    fontWeight: 'bold',
  },
});

export default App;
