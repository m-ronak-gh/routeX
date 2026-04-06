import osmnx as ox
import struct
import sys

# Using structured queries for smaller, faster-to-process cities
CITIES = {
    "cambridge": {"city": "Cambridge", "country": "UK"},
    "pasadena": {"city": "Pasadena", "state": "California", "country": "USA"},
    "nara": {"city": "Nara", "country": "Japan"}
}

def fetch_and_export(city_key):
    if city_key not in CITIES:
        print(f"Error: '{city_key}' not found in CITIES dictionary.")
        return

    place = CITIES[city_key]
    # Format a nice print string from the dictionary
    place_name = ", ".join(place.values())
    print(f"Downloading {place_name}...")

    # Download drivable street network
    G = ox.graph_from_place(place, network_type="drive")
    G = ox.add_edge_speeds(G)        # adds speed_kph attribute
    G = ox.add_edge_travel_times(G)  # adds travel_time attribute

    # Project to UTM for accurate distances
    G_proj = ox.project_graph(G)

    nodes, edges = ox.graph_to_gdfs(G_proj)
    print(f"Nodes: {len(nodes)}, Edges: {len(edges)}")

    # Build compact ID mapping (osmnx uses OSM node IDs which are large)
    node_ids = list(nodes.index)
    id_map = {osm_id: i for i, osm_id in enumerate(node_ids)}

    out_path = f"data/{city_key}.bin"
    with open(out_path, "wb") as f:
        # Header: node count, edge count
        n_nodes = len(node_ids)
        n_edges = len(edges)
        f.write(struct.pack("II", n_nodes, n_edges))

        # Nodes: lat, lon (as doubles) - use original (non-projected) coords
        nodes_orig, _ = ox.graph_to_gdfs(G)
        for osm_id in node_ids:
            row = nodes_orig.loc[osm_id]
            lat = float(row["y"])
            lon = float(row["x"])
            f.write(struct.pack("dd", lat, lon))

        # Edges: from_id, to_id (compact ints), weight in metres
        for (u, v, _), row in edges.iterrows():
            if u not in id_map or v not in id_map:
                continue
            from_id = id_map[u]
            to_id = id_map[v]
            length = float(row.get("length", 1.0))
            f.write(struct.pack("IId", from_id, to_id, length))

    print(f"Written to {out_path}")

if __name__ == "__main__":
    city = sys.argv[1] if len(sys.argv) > 1 else "cambridge"
    fetch_and_export(city)